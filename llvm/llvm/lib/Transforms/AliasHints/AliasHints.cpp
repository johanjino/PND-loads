#include "llvm/Transforms/AliasHints/AliasHints.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include <cstdint>
#include <vector>

PreservedAnalyses AliasHintsPass::run(LoopNest &LN, LoopAnalysisManager &AM,
                                      LoopStandardAnalysisResults &AR, LPMUpdater &U){
    Function &F = *LN.getParent();
    DependenceInfo DI = DependenceInfo(&F, &AR.AA, &AR.SE, &AR.LI);

    markLoads(LN, DI, AR);

    return PreservedAnalyses::all();
}

void AliasHintsPass::markLoads(LoopNest &LN, DependenceInfo &DI, LoopStandardAnalysisResults &AR){
    SmallVector<LoadInst *> all_loads;
    SmallVector<StoreInst *> all_stores;
    SmallVector<CallInst *> all_calls;

    for (auto B: LN.getOutermostLoop().getBlocks()){
        for (Instruction *I = B->getFirstNonPHI(), *Term = B->getTerminator();
                I != Term; I = I->getNextNonDebugInstruction()){
            if (isa<LoadInst>(I)) all_loads.push_back(reinterpret_cast<LoadInst *>(I));
            else if (isa<StoreInst>(I)) all_stores.push_back(reinterpret_cast<StoreInst *>(I));
            else if (isa<CallInst>(I)) all_calls.push_back(reinterpret_cast<CallInst *>(I));
        }
    }

    //LI.GeneratedChecks obtained through horribly hacky means
    SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs = findVersionedLoops(LN, AR.LI.GeneratedChecks, AR.LI, AR.DT);

    /* Iterate over all loads using our own method for finding labels */
    AliasHint Hint;
    std::set<LoadInst *> PNDLoads;
    for (auto Load: all_loads){
        Hint = determineHint(Load, all_stores, all_calls, VersionPairs, DI, AR.SE, AR.AA, AR.LI);
        if(Hint == AliasHint::PredictNone)
            PNDLoads.insert(Load);
    }

    /* Then for each innermost loop use the vectorisation loop access analysis
     * for catching more no dependency cases and forward dependencies */
    std::set<LoadInst *> LAIPNDLoads;
    for (auto Loop: LN.getLoops()){
        //may be multiple innermost loops
        if (Loop->isInnermost()){
            LoopAccessInfo LAI = LoopAccessInfo(Loop, &(AR.SE), &(AR.TLI), &(AR.AA), &(AR.DT), &(AR.LI));
            MemoryDepChecker MDC = LAI.getDepChecker();
            for (auto Query: MDC.QueryResults){
                LoadInst *Load = reinterpret_cast<LoadInst *>(Query.first);
                MemoryDepChecker::Dependence::DepType Type = Query.second;
                if (Type == MemoryDepChecker::Dependence::NoDep ||
                    Type == MemoryDepChecker::Dependence::Forward){
                    LAIPNDLoads.insert(Load);
                }
        }
    }

    errs() << "Test\n";
    if (!std::includes(PNDLoads.begin(), PNDLoads.begin(), LAIPNDLoads.begin(), LAIPNDLoads.end())){
        errs() << "Own pass catches no deps that LAI doesn't\n";
    }

    std::set<LoadInst *> AllPNDLoads = PNDLoads;
    AllPNDLoads.insert(LAIPNDLoads.begin(), LAIPNDLoads.end());
    for (auto Load: AllPNDLoads){
        changeAddrSpace(Load, PREDICT_NO_ALIAS_ADDRESS_SPACE);
    }

}

AliasHint AliasHintsPass::determineHint(LoadInst *Load, SmallVector<StoreInst *> all_stores,
                                        SmallVector<CallInst *> all_calls, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, DependenceInfo DI, ScalarEvolution &SE,
                                        AAResults &AA, LoopInfo &LI){
    std::string operand;
    llvm::raw_string_ostream operand_buffer(operand);
    std::unique_ptr<Dependence> Dep;
    AliasHint Hint;
    bool foundUnpredictableDep = false;
    for (auto Store: all_stores){
        if (!withinSameVersion(Load, Store, VersionPairs, LI)) continue;
        Dep = DI.depends(Store, Load, true);
        if (!Dep) continue;
        Hint = isProblematicDep(Load, Dep.get(), LI, SE, AA);
        if (Hint == AliasHint::Predict) return Hint;
        if (Hint == AliasHint::Unchanged) foundUnpredictableDep = true;
    }
    if (foundUnpredictableDep) return AliasHint::Unchanged;
    for (auto Call: all_calls){
        if (!withinSameVersion(Load, Call, VersionPairs, LI)) continue;
        Dep = DI.depends(Call, Load, true);
        if (!Dep) continue;
        Hint = isProblematicDep(Load, Dep.get(), LI, SE, AA);
        if (Hint != AliasHint::PredictNone) return AliasHint::Unchanged; //going by heuristic that modref results often don't fit into the behavior we intend for Predict Alias, and will also often be unknown over approximations
    }
    return AliasHint::PredictNone;
}

/* loop access analysis - run loop_analysis and create a list of loads with nodep or forward dep in the class, then read from them and mark */

AliasHint AliasHintsPass::isProblematicDep(LoadInst *Load, Dependence *Dep, LoopInfo &LI, ScalarEvolution &SE, AAResults &AA){
    Instruction *Source = Dep->getSrc();
    if (Dep->isConfused()) return AliasHint::Unchanged;
    if (Dep->isOrdered()){
        //a call based dep is too unpredictable to make fine grained decisions on past the point of an ordered dep existing at all
        if (isa<CallInst>(Source)) return AliasHint::Unchanged;
        assert(isa<LoadInst>(Dep->getDst()) && "This function assumes loads are always placed at dep dst.\n");
        assert(isa<StoreInst>(Source) && "This function only supports calls and stores.\n");
        StoreInst *Store = reinterpret_cast<StoreInst *>(Source);
        unsigned LoadDepth = LI.getLoopFor(Load->getParent())->getLoopDepth();
        unsigned StoreDepth = LI.getLoopFor(Store->getParent())->getLoopDepth();
        Dep->normalize(&SE);
        unsigned Levels = Dep->getLevels();
        //if cross-loop dep we can reason about instruction distances. this assumes loops are generally large and small kernels.
        //1. one nest apart - assume within same window
        //2. two nests apart - a wrong guess either way here is very bad, leave unchanged
        //2 b. in different sub-trees of the loop structure - e.g. load loop !contain store loop, assume not in window
        //3. three or more - assume not within window
        if (LoadDepth != StoreDepth){
            int Diff = std::abs((int)LoadDepth - (int)StoreDepth);
            if (Diff == 1) {
                if (AA.alias(Load->getPointerOperand(), Store->getPointerOperand()) != AliasResult::MayAlias) return AliasHint::Predict;
                return AliasHint::Unchanged;
            }
            if (Diff == 2){
                if (Levels == std::min(LoadDepth, StoreDepth)) return AliasHint::Unchanged;
                return AliasHint::PredictNone;
            }
            if (Diff > 2) return AliasHint::PredictNone;
        }
        //if within the same loop it is simpler to reason about dep distances
        const SCEV *scev_distance;
        if (Levels > 2){
            //does there exist a distance greater than one at a level greater than two nests above the dep?
            for (unsigned Level=1; Level < Levels-1; Level++){
                uint64_t distance;
                scev_distance = Dep->getDistance(Level);
                if (!scev_distance) continue;
                distance = SE.getUnsignedRangeMin(scev_distance).getZExtValue();
                if (Levels - Level > 2 && distance >= 1) return AliasHint::PredictNone;
                if (distance >= 2) return AliasHint::PredictNone;
            }
        }
        if (AA.alias(Load->getPointerOperand(), Store->getPointerOperand()) != AliasResult::MayAlias) return AliasHint::Predict;
        return AliasHint::Unchanged;
    }
    //dep is unordered
    return AliasHint::PredictNone;
}

SmallVector<std::pair<Loop *, Loop *>, 2> AliasHintsPass::findVersionedLoops(LoopNest &LN, SmallVector<BasicBlock *, 1> GeneratedChecks, LoopInfo &LI, DominatorTree &DT){

    SmallVector<std::pair<Loop *, Loop *>, 2> Result;

    for (auto MemCheckBlock: GeneratedChecks){
        Loop *L0 = LI.getLoopFor(MemCheckBlock);
        if (!L0) continue;
        unsigned Depth = L0->getLoopDepth() + 1;
        Loop *L1, *L2;
        BranchInst *CondBr = reinterpret_cast<BranchInst *>(MemCheckBlock->getTerminator());
        BasicBlock *B1 = CondBr->getSuccessor(0);
        BasicBlock *B2 = CondBr->getSuccessor(1);
        LoopVectorTy Loops = LN.getLoopsAtDepth(Depth);
        for (auto Loop: Loops){
            BasicBlock *Preheader = Loop->getLoopPreheader();
            if (DT.dominates(B1, Preheader)) L1 = Loop;
            else if (DT.dominates(B2, Preheader)) L2 = Loop;
        }
        Result.push_back(std::pair<Loop *, Loop*>(L1, L2));
    }

    return Result;
}

//For a pair of loops separated by a vectorising runtime check, are a given load and given store/call dominated by the same one? if not, they exist on opposite sides of the runtime check and shouldn't be compared
bool AliasHintsPass::withinSameVersion(LoadInst *Load, Instruction *DepInst, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, LoopInfo &LI){
    Loop *L1 = LI.getLoopFor(Load->getParent());
    Loop *L2 = LI.getLoopFor(DepInst->getParent());

    for (auto VersionPair: VersionPairs)
        if ((VersionPair.first->contains(L1) && VersionPair.second->contains(L2))
            || (VersionPair.first->contains(L2) && VersionPair.second->contains(L1)))
            return false;

    return true;
}


void AliasHintsPass::changeAddrSpace(LoadInst *Load, unsigned int AddrSpace){
    Value *Ptr = Load->getPointerOperand();
    Type *ElemTy = Ptr->getType()->getScalarType();
    PointerType *DestTy = PointerType::get(ElemTy, AddrSpace);
    IRBuilder<> Builder(Load);
    Value *CastPtr = Builder.CreateAddrSpaceCast(Ptr, DestTy);
    Load->setOperand(0, CastPtr);
}

llvm::PassPluginLibraryInfo getAliasHintsPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "AliasHints", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerLateLoopOptimizationsEPCallback(
                [](llvm::LoopPassManager &LPM, OptimizationLevel Level) {
                  LPM.addPass(AliasHintsPass());
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::LoopPassManager &LPM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "aliashints") {
                    LPM.addPass(AliasHintsPass());
                    return true;
                  }
                  return false;
                });
          }};
}

#ifndef LLVM_ALIASHINTSPASS_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getAliasHintsPassPluginInfo();
}
#endif
