#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Transforms/AliasHints/AliasHints.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/IR/PassManagerImpl.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <cstdio>

PreservedAnalyses AliasHintsPass::run(LoopNest &LN, LoopAnalysisManager &AM,
                                      LoopStandardAnalysisResults &AR, LPMUpdater &U){
    Function &F = *LN.getParent();
    LLVMContext &Ctx = F.getContext();
    DependenceInfo DI = DependenceInfo(&F, &AR.AA, &AR.SE, &AR.LI);
    
    bool withInstrumentation = true;

    markLoads(LN, DI, AR, Ctx, withInstrumentation);

    return PreservedAnalyses::all();
}

void AliasHintsPass::markConstantAccesses(Function &F, AAResults &AA, LLVMContext &Ctx){
    std::vector<LoadInst *> constant_loads;
    for (Function::iterator b = F.begin(), be = F.end(); b != be; ++b){
        BasicBlock &B = *b;
        for (Instruction *I = B.getFirstNonPHI(), *Term = B.getTerminator();
            I != Term; I = I->getNextNonDebugInstruction()){
            if (isa<LoadInst>(I)){
                LoadInst *L = reinterpret_cast<LoadInst *>(I);
                if (AA.pointsToConstantMemory(L->getPointerOperand()))
                    constant_loads.push_back(L);
            }
        }
    }

    for (auto L: constant_loads)    {
        AAMDNodes AAInfo = L->getAAMetadata();
        AAInfo.PND = MDNode::get(Ctx, ArrayRef<Metadata*>());
        L->setAAMetadata(AAInfo);
    }
}

void getAliasMap(AliasMapType& AliasMap){

    std::string PNDProfileFilename = std::getenv("AliasHintsPGOFile") ? std::getenv("AliasHintsPGOFile") : "";
    // std::string PNDProfileFilename = "/work/johan/PND-Loads/output.txt";

    if (PNDProfileFilename == ""){
        errs() <<  "Env variable not set \n";
        return;
    }
    std::ifstream file(PNDProfileFilename);
    if (!file) {
        errs() << "Error opening file: " << PNDProfileFilename << "\n";
        return;
    }
    // errs() << "Using File: " << PNDProfileFilename << "\n";

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string loadId;
        unsigned long long execCount;

        // Read Load (file:line:col)
        stream >> loadId >> execCount;

        // Read Store
        std::unordered_map<std::string, unsigned long long> dependantStores;
        std::string storeId;
        unsigned long long aliasCount;
        while (stream >> storeId) {
            stream >> aliasCount;
            // += incase of multiple entries
            dependantStores[storeId] += aliasCount;
        }

        AliasMap[loadId] = dependantStores;

        // Hack: For now store exec count of lond in the same map:
        // += incase of multiple entries
        AliasMap[loadId]["execCount"] += execCount;
    }
}


void getSVFResult(std::unordered_map<std::string, std::unordered_set<std::string>>& SVFAliasMap){

    std::string SVFResultsFile = std::getenv("SVFResultsFile") ? std::getenv("SVFResultsFile") : "";

    if (SVFResultsFile == ""){
        errs() <<  "SVFResultsFile Env variable not set \n";
        return;
    }
    std::ifstream file(SVFResultsFile);
    if (!file) {
        errs() << "Error opening file: " << SVFResultsFile << "\n";
        return;
    }
    // errs() << "Using File: " << SVFResultsFile << "\n";

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string val, first, second;
        // Read two parts
        // Val = 0 for NoAlias
        // Val = 1 for MayAlias
        stream >> val >> first >> second;
        std::string pair1 = first + " " + second;
        
        std::string third, fourth;
        stream >> third >> fourth;
        std::string pair2 = third + " " + fourth + " " + val;
        SVFAliasMap[pair1].insert(pair2);
    }
}


void AliasHintsPass::markLoads(LoopNest &LN,
                               DependenceInfo &DI,
                               LoopStandardAnalysisResults &AR,
                               LLVMContext &Ctx,
                               bool withInstrumentation){
    SmallVector<LoadInst *> all_loads;
    SmallVector<StoreInst *> all_stores;
    SmallVector<CallInst *> all_calls;

    std::map<Loop *, LoopAccessInfo *> LAIInstances;

    for (auto B: LN.getOutermostLoop().getBlocks()){
        for (Instruction *I = B->getFirstNonPHI(), *Term = B->getTerminator();
                I != Term; I = I->getNextNonDebugInstruction()){
            if (isa<LoadInst>(I)) all_loads.push_back(reinterpret_cast<LoadInst *>(I));
            else if (isa<StoreInst>(I)) all_stores.push_back(reinterpret_cast<StoreInst *>(I));
            else if (isa<CallInst>(I)) all_calls.push_back(reinterpret_cast<CallInst *>(I));
        }
    }


    for (auto Loop: LN.getLoops()){
        //may be multiple innermost loops
        if (Loop->isInnermost()){
            LoopAccessInfo *LAI = new LoopAccessInfo(Loop, &(AR.SE), &(AR.TLI), &(AR.AA), &(AR.DT), &(AR.LI));
            LAIInstances[Loop] = LAI;
        }
    }

    //LI.GeneratedChecks obtained through horribly hacky means
    SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs = findVersionedLoops(LN, AR.LI.GeneratedChecks, AR.LI, AR.DT);

    /* Iterate over all loads using our own method for finding labels */
    markConstantAccesses(*LN.getParent(), AR.AA, Ctx);
    AliasHint Hint;
    AliasMapType AliasMap;
    if (withInstrumentation){
        getAliasMap(AliasMap);
    }
    std::unordered_map<std::string, std::unordered_set<std::string>> SVFAliasMap;
    bool useSVFResults = true;
    if (useSVFResults){
        getSVFResult(SVFAliasMap);
    }
    for (auto Load: all_loads){
        if (!Load->isSimple()) continue;
        Hint = determineHint(Load,
                             all_stores,
                             all_calls,
                             LAIInstances,
                             VersionPairs,
                             DI,
                             AR.SE,
                             AR.AA,
                             AR.LI,
                             AliasMap,
                             SVFAliasMap,
                             withInstrumentation);
        if(Hint == AliasHint::PredictNone) {
            AAMDNodes AAInfo = Load->getAAMetadata();
            AAInfo.PND = MDNode::get(Ctx, ArrayRef<Metadata*>());
            Load->setAAMetadata(AAInfo);
        }
        else if (Hint == AliasHint::Unchanged && Load->getAAMetadata().PND) {
            //We found that a load labelled on a previous invocation of the pass
            //shouldn't be labelled thanks to more available information
            AAMDNodes AAInfo = Load->getAAMetadata();
            AAInfo.PND = nullptr;
            Load->setAAMetadata(AAInfo);
        }
    }

    for (auto LAI: LAIInstances){
        delete LAI.second;
    }

}

//TODO: replace with either IndirectUnsafe or dep.isScalar
bool isVariantAtAllLevels(Value *Ptr, Loop *ParentLoop, LoopInfo &LI, ScalarEvolution &SE){
    Loop *L = ParentLoop;
    Value *NextValue = Ptr;
    Value *PreviousValue = nullptr;
    while ((L = L->getParentLoop())) {
        bool nest_level_is_variant = false;
        for (; !nest_level_is_variant; NextValue = getUnderlyingObject(NextValue, 1)){
            //getUnderlyingObject is giving up on something that isn't a load so we're giving up too
            if (NextValue == PreviousValue) return false;
            PreviousValue = NextValue;

            //we haven't worked through the whole nest yet but the underlying
            //value is already a function argument or global variable, must be invariant
            if (!isa<Instruction>(NextValue)) return false;

            Instruction *NextInstruction = reinterpret_cast<Instruction *>(NextValue);

            //found instruction on the ptr at this specific loop, check if its invariant
            //if it is, keep stripping back instructions
            if (L->contains(NextInstruction) && L == LI.getLoopFor(NextInstruction->getParent())){
                const SCEV *Scev = SE.getSCEV(NextInstruction);
                if (!SE.isLoopInvariant(Scev, L)) nest_level_is_variant = true;
            }
            //found instruction outside this level before we found any instructions within it,
            //pointer is invariant w.r.t this level
            else if (!L->contains(NextInstruction)) return false;

            if (auto *Load = dyn_cast<LoadInst>(NextValue)) NextValue = Load->getPointerOperand();
        }
    }
    return true;
}

static bool canComputePointerDiff(ScalarEvolution &SE,
                                  const SCEV *A, const SCEV *B) {
    if (SE.getEffectiveSCEVType(A->getType()) !=
        SE.getEffectiveSCEVType(B->getType()))
    return false;

    return SE.instructionCouldExistWithOperands(A, B);
}

//FIXME
bool isSeparateCacheLine(Instruction *Load, Instruction *Store, ScalarEvolution &SE, AAResults &AA){

    LoadInst *L;
    StoreInst *S;
    if (isa<LoadInst>(Load)) L = reinterpret_cast<LoadInst *>(Load);
    else { errs() << "Load argument not a load!\n"; exit(1); }
    if (isa<StoreInst>(Store)) S = reinterpret_cast<StoreInst *>(Store);
    else { errs() << "Store argument not a store!\n"; exit(1); }

    MemoryLocation LocA = MemoryLocation(
        L->getPointerOperand(),
        LocationSize::precise(CACHE_LINE_SIZE),
        L->getAAMetadata());
    MemoryLocation LocB = MemoryLocation(
        S->getPointerOperand(),
        LocationSize::precise(CACHE_LINE_SIZE),
        S->getAAMetadata());

    if (AA.isNoAlias(LocA, LocB)) return true;

    //kind of insane, but just copy pasting SCEV AA here because I don't want to figure out how to
    //plug into it in a nice way and not sure how to enable it from clang
    if (LocA.Size.isZero() || LocB.Size.isZero())
    return true;

    // This is SCEVAAResult. Get the SCEVs!
    const SCEV *AS = SE.getSCEV(const_cast<Value *>(LocA.Ptr));
    const SCEV *BS = SE.getSCEV(const_cast<Value *>(LocB.Ptr));

    // If they evaluate to the same expression, it's a MustAlias.
    if (AS == BS)
    return false;

    // If something is known about the difference between the two addresses,
    // see if it's enough to prove a NoAlias.
    if (canComputePointerDiff(SE, AS, BS)) {
    unsigned BitWidth = SE.getTypeSizeInBits(AS->getType());
    APInt ASizeInt(BitWidth, LocA.Size.hasValue()
                                    ? LocA.Size.getValue()
                                    : MemoryLocation::UnknownSize);
    APInt BSizeInt(BitWidth, LocB.Size.hasValue()
                                    ? LocB.Size.getValue()
                                    : MemoryLocation::UnknownSize);

    // Compute the difference between the two pointers.
    const SCEV *BA = SE.getMinusSCEV(BS, AS);

    // Test whether the difference is known to be great enough that memory of
    // the given sizes don't overlap. This assumes that ASizeInt and BSizeInt
    // are non-zero, which is special-cased above.
    if (!isa<SCEVCouldNotCompute>(BA) &&
        ASizeInt.ule(SE.getUnsignedRange(BA).getUnsignedMin()) &&
        (-BSizeInt).uge(SE.getUnsignedRange(BA).getUnsignedMax()))
        return true;

    // Folding the subtraction while preserving range information can be tricky
    // (because of INT_MIN, etc.); if the prior test failed, swap AS and BS
    // and try again to see if things fold better that way.

    // Compute the difference between the two pointers.
    const SCEV *AB = SE.getMinusSCEV(AS, BS);

    // Test whether the difference is known to be great enough that memory of
    // the given sizes don't overlap. This assumes that ASizeInt and BSizeInt
    // are non-zero, which is special-cased above.
    if (!isa<SCEVCouldNotCompute>(AB) &&
        BSizeInt.ule(SE.getUnsignedRange(AB).getUnsignedMin()) &&
        (-ASizeInt).uge(SE.getUnsignedRange(AB).getUnsignedMax()))
        return true;
    }

    return false;
}

bool checkInstrumentationInfo(LoadInst *Load, StoreInst *Store, AliasMapType& AliasMap, double threshold){
    // const Function *F1 = Load->getFunction();
    // const Function *F2 = Store->getFunction();
    // // Checking if load and store in same function. This actually doesnt change anything
    // // Should remove this after verifing
    // if (F1!=F2){
    //     return false;
    // }
    
    // Get the debug locations
    const DebugLoc &LDL = Load->getDebugLoc();
    const DebugLoc &SDL = Store->getDebugLoc();

    // Check if both locations are valid
    if (LDL && SDL) {
        std::string loadLoc = std::to_string(LDL.getLine()) + ":" + std::to_string(LDL.getCol());
        std::string storeLoc = std::to_string(SDL.getLine()) + ":" + std::to_string(SDL.getCol());
        
        DILocation *Loc;
        DIScope *Scope;
        DIFile *File;

        Loc = LDL.get();
        Scope = Loc->getScope();
        File = Scope->getFile();
        std::string loadId = File->getFilename().str() + ":" + loadLoc;

        Loc = SDL.get();
        Scope = Loc->getScope();
        File = Scope->getFile();
        std::string storeId = File->getFilename().str() + ":" + storeLoc;
        

        // Check alias here:
        // errs() << loadId << " - " << storeId<< "\n";
        if (AliasMap.find(loadId) != AliasMap.end()){
            if (AliasMap[loadId].find(storeId) != AliasMap[loadId].end()){
                // ToDo:
                // Why are we checking only percentage
                double alias_count = AliasMap[loadId][storeId];
                double exec_count = AliasMap[loadId]["execCount"];
                double percentage = (alias_count*100)/exec_count;
                if (percentage > threshold){
                    errs() << "\nSUCCESS.. Caught May Alias from being marked PND\n";
                    errs() << loadId << " " << storeId << " - " << exec_count<< "\n";
                    errs() << percentage << "\n";
                    return true;
                }
                // errs() << "\nHMM WHYY??..\n";
                // errs() << loadId << " " << storeId << " - " << exec_count<< "\n";
                // errs() << percentage << "\n";
            }
            // errs() << "\nLoad Found but not alias\n";
            // errs() << loadId << " " << storeId << " - " << AliasMap[loadId]["execCount"] << "\n";
            return false;
        }
    }
    // errs() << "Reached here 1?\n";
    return false;
}


AliasResult checkSVFInfo(LoadInst *Load, StoreInst *Store, std::unordered_map<std::string, std::unordered_set<std::string>> &SVFNoAliasMap){
    
    // Get the debug locations
    const DebugLoc &LDL = Load->getDebugLoc();
    const DebugLoc &SDL = Store->getDebugLoc();

    // Check if both locations are valid
    if (LDL && SDL) {
        std::string loadLoc = std::to_string(LDL.getLine()) + ":" + std::to_string(LDL.getCol());
        std::string storeLoc = std::to_string(SDL.getLine()) + ":" + std::to_string(SDL.getCol());
        DILocation *Loc = LDL.get();
        // Get the scope (DIScope) of the location
        DIScope *Scope = Loc->getScope();
        // Retrieve the file name and directory
        DIFile *File = Scope->getFile();
        std::string FileLoc = File->getFilename().str();
        
        std::string LoadInfo = FileLoc + " " + loadLoc;
        std::string StoreInfo = FileLoc + " " + storeLoc;

        auto it = SVFNoAliasMap.find(LoadInfo);
        if (it != SVFNoAliasMap.end()) {
            if(it->second.count(StoreInfo+" 2")){
                // errs () << "SVF caught Must Alias\n";
                return AliasResult::MustAlias;
            }
            else if (it->second.count(StoreInfo+" 1")){
                // errs () << "SVF caught May Alias\n";
                return AliasResult::MayAlias;
            }
            else if(it->second.count(StoreInfo+" 0")){
                // errs () << "SVF caught No Alias\n";
                return AliasResult::NoAlias;
            }
        }
    }
    return AliasResult::MayAlias;
}


AliasHint AliasHintsPass::determineHint(LoadInst *Load, SmallVector<StoreInst *> all_stores,
                                        SmallVector<CallInst *> all_calls, std::map<Loop *, LoopAccessInfo *> LAIInstances, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, DependenceInfo DI, ScalarEvolution &SE,
                                        AAResults &AA, LoopInfo &LI, AliasMapType &AliasMap, std::unordered_map<std::string, std::unordered_set<std::string>> &SVFAliasMap, bool withInstrumentation){
    
    std::unique_ptr<Dependence> Dep;
    Loop *current_loop = LI.getLoopFor(Load->getParent());
    std::map<Instruction *, MemoryDepChecker::Dependence::DepType> store_map;
    bool was_LAI_analysed = false;
    if (current_loop->isInnermost()){
        LoopAccessInfo *LAI = LAIInstances[current_loop];
        MemoryDepChecker MDC = LAI->getDepChecker();
        if (MDC.QueryResults.find(Load) != MDC.QueryResults.end()){
            was_LAI_analysed = true;
            store_map = MDC.QueryResults[Load];
        }
    }
    
    // Threshold for when to not mark PND
    double threshold = 0;

    // unsigned aliascount = 0;
    for (auto Store: all_stores){
        auto AAresult = AA.alias(Store->getPointerOperand(), Load->getPointerOperand());
        auto AASVFresult = checkSVFInfo(Load, Store, SVFAliasMap);
        auto FinalResult = AliasResult::MayAlias;
        if (AAresult == AliasResult::NoAlias || AASVFresult == AliasResult::NoAlias){
            FinalResult = AliasResult::NoAlias;
        }
        else if (AAresult == AliasResult::MustAlias || AASVFresult == AliasResult::MustAlias || AAresult == AliasResult::PartialAlias){
            FinalResult = AliasResult::MustAlias;
        }
        else if (AAresult == AliasResult::MayAlias && AASVFresult == AliasResult::MayAlias){
            FinalResult = AliasResult::MayAlias;
        }
        else{
            errs () << "Shouldn't happen\n";
        }

        //Debug
        if (AAresult == AliasResult::MayAlias && AASVFresult == AliasResult::NoAlias) errs () << "SVF caught No Alias that LLVM didnt\n";

        if (FinalResult==AliasResult::NoAlias) continue;
        // if (FinalResult==AliasResult::NoAlias || FinalResult==AliasResult::MayAlias || FinalResult==AliasResult::PartialAlias || FinalResult==AliasResult::MustAlias) continue;
        if (FinalResult==AliasResult::MayAlias && withInstrumentation && !checkInstrumentationInfo(Load, Store, AliasMap, threshold)) continue;
        else {
            return AliasHint::Unchanged;
        }
        if (!withinSameVersion(Load, Store, VersionPairs, LI)){
            continue;
        }

        Dep = DI.depends(Store, Load, true);
        if (!Dep){
           continue;
        }
        if (!isProblematicDep(Load, Dep.get(), LI, SE, AA)){ 
            continue;
        };
        if (was_LAI_analysed && LI.getLoopFor(Store->getParent()) == current_loop &&
            Store->isSimple() && store_map.find(Store) != store_map.end()){
            MemoryDepChecker::Dependence::DepType Type = store_map[Store];
            switch (Type){
                case MemoryDepChecker::Dependence::NoDep:
                case MemoryDepChecker::Dependence::IndependentStride:
                case MemoryDepChecker::Dependence::SafeDistance: //maybe need to do forward checks?
                    continue;
                case MemoryDepChecker::Dependence::Forward:
                case MemoryDepChecker::Dependence::ForwardButPreventsForwarding:
                    if (isVariantAtAllLevels(Load->getPointerOperand(), current_loop, LI, SE) &&
                        isSeparateCacheLine(Load, Store, SE, AA))
                        continue;
                    return AliasHint::Unchanged;
                default:
                    return AliasHint::Unchanged;
            }
        }
        else {
            return AliasHint::Unchanged;
        }
    }

    for (auto Call: all_calls){
        if (!withinSameVersion(Load, Call, VersionPairs, LI)) continue;
        ModRefInfo res = AA.getModRefInfo(Load, Call);
        if (isModSet(res)) return AliasHint::Unchanged;
    }
    
    return AliasHint::PredictNone;
}

bool AliasHintsPass::isProblematicDep(LoadInst *Load, Dependence *Dep, LoopInfo &LI, ScalarEvolution &SE, AAResults &AA){
    if (Dep->isConfused()) return true;
    if (Dep->isOrdered()){
        Loop *LoadLoop = LI.getLoopFor(Load->getParent());
        unsigned LoadDepth = LoadLoop->getLoopDepth();
        Instruction *Source = Dep->getSrc();
        StoreInst *Store = reinterpret_cast<StoreInst *>(Source);
        unsigned StoreDepth = LI.getLoopFor(Store->getParent())->getLoopDepth();
        Dep->normalize(&SE);
        unsigned Levels = Dep->getLevels();
        if ((Dep->isPeelFirst(LoadDepth) || Dep->isPeelLast(LoadDepth)) && LoadLoop->isOutermost()) return false;
        //if cross-loop dep we can reason about instruction distances. this assumes loops are generally large and small kernels.
        if (LoadDepth != StoreDepth){
            int Diff = std::abs((int)LoadDepth - (int)StoreDepth);
            if (Diff == 1) return true;
            if (Diff == 2){
                if (Levels == std::min(LoadDepth, StoreDepth)) return true;
                return false;
            }
            if (Diff > 2) return false;
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
                if (Levels - Level > 2 && distance >= 1) return false;
                if (distance >= 2) return false;
            }
        }
        return true;
    }
    //dep is unordered
    return false;
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




// #include "llvm/Analysis/ValueTracking.h"
// #include "llvm/Transforms/AliasHints/AliasHints.h"
// #include "llvm/Analysis/AliasAnalysis.h"
// #include "llvm/Analysis/LoopInfo.h"
// #include "llvm/Analysis/MemoryLocation.h"
// #include "llvm/Transforms/Vectorize/LoopVectorize.h"
// #include "llvm/IR/PassManagerImpl.h"
// #include <cstdint>
// #include <vector>
// #include <unordered_map>
// #include <fstream>
// #include <sstream>
// #include <cstdio>

// PreservedAnalyses AliasHintsPass::run(LoopNest &LN, LoopAnalysisManager &AM,
//                                       LoopStandardAnalysisResults &AR, LPMUpdater &U){
//     Function &F = *LN.getParent();
//     LLVMContext &Ctx = F.getContext();
//     DependenceInfo DI = DependenceInfo(&F, &AR.AA, &AR.SE, &AR.LI);
    
//     bool withInstrumentation = true;

//     markLoads(LN, DI, AR, Ctx, withInstrumentation);

//     return PreservedAnalyses::all();
// }

// void AliasHintsPass::markConstantAccesses(Function &F, AAResults &AA, LLVMContext &Ctx){
//     std::vector<LoadInst *> constant_loads;
//     for (Function::iterator b = F.begin(), be = F.end(); b != be; ++b){
//         BasicBlock &B = *b;
//         for (Instruction *I = B.getFirstNonPHI(), *Term = B.getTerminator();
//             I != Term; I = I->getNextNonDebugInstruction()){
//             if (isa<LoadInst>(I)){
//                 LoadInst *L = reinterpret_cast<LoadInst *>(I);
//                 if (AA.pointsToConstantMemory(L->getPointerOperand()))
//                     constant_loads.push_back(L);
//             }
//         }
//     }

//     for (auto L: constant_loads)    {
//         AAMDNodes AAInfo = L->getAAMetadata();
//         AAInfo.PND = MDNode::get(Ctx, ArrayRef<Metadata*>());
//         L->setAAMetadata(AAInfo);
//     }
// }

// void getAliasMap(AliasMapType& AliasMap){

//     std::string PNDProfileFilename = std::getenv("AliasHintsPGOFile") ? std::getenv("AliasHintsPGOFile") : "";
//     // std::string PNDProfileFilename = "/work/johan/PND-Loads/output.txt";

//     if (PNDProfileFilename == ""){
//         errs() <<  "Env variable not set \n";
//         return;
//     }
//     std::ifstream file(PNDProfileFilename);
//     if (!file) {
//         errs() << "Error opening file: " << PNDProfileFilename << "\n";
//         return;
//     }
//     errs() << "Using File: " << PNDProfileFilename << "\n";

//     std::string line;
//     while (std::getline(file, line)) {
//         std::istringstream stream(line);
//         std::string loadKeyword, filePath, loadLoc;
//         unsigned long long execCount;
        

//         // Read the file path (it's the part after the first line:col)
//         stream >> filePath;
//         stream >> loadLoc; // "<line:col>"

//         // Read Load Exec Count
//         stream >> execCount;

//         // Read store line:col parts
//         std::unordered_map<std::string, unsigned long long> dependantStores;
//         std::string storeLoc;
//         unsigned long long aliasCount;
//         while (stream >> storeLoc) {
//             stream >> aliasCount;
//             // += incase of multiple entries
//             dependantStores[storeLoc] += aliasCount;
//         }

//         AliasMap[filePath][loadLoc] = dependantStores;

//         // Hack: For now store exec count of lond in the same map:
//         // += incase of multiple entries
//         AliasMap[filePath][loadLoc]["execCount"] += execCount;
//     }
// }

// void AliasHintsPass::markLoads(LoopNest &LN,
//                                DependenceInfo &DI,
//                                LoopStandardAnalysisResults &AR,
//                                LLVMContext &Ctx,
//                                bool withInstrumentation){
//     SmallVector<LoadInst *> all_loads;
//     SmallVector<StoreInst *> all_stores;
//     SmallVector<CallInst *> all_calls;

//     std::map<Loop *, LoopAccessInfo *> LAIInstances;

//     for (auto B: LN.getOutermostLoop().getBlocks()){
//         for (Instruction *I = B->getFirstNonPHI(), *Term = B->getTerminator();
//                 I != Term; I = I->getNextNonDebugInstruction()){
//             if (isa<LoadInst>(I)) all_loads.push_back(reinterpret_cast<LoadInst *>(I));
//             else if (isa<StoreInst>(I)) all_stores.push_back(reinterpret_cast<StoreInst *>(I));
//             else if (isa<CallInst>(I)) all_calls.push_back(reinterpret_cast<CallInst *>(I));
//         }
//     }


//     for (auto Loop: LN.getLoops()){
//         //may be multiple innermost loops
//         if (Loop->isInnermost()){
//             LoopAccessInfo *LAI = new LoopAccessInfo(Loop, &(AR.SE), &(AR.TLI), &(AR.AA), &(AR.DT), &(AR.LI));
//             LAIInstances[Loop] = LAI;
//         }
//     }

//     //LI.GeneratedChecks obtained through horribly hacky means
//     SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs = findVersionedLoops(LN, AR.LI.GeneratedChecks, AR.LI, AR.DT);

//     /* Iterate over all loads using our own method for finding labels */
//     markConstantAccesses(*LN.getParent(), AR.AA, Ctx);
//     AliasHint Hint;
//     AliasMapType AliasMap;
//     if (withInstrumentation){
//         getAliasMap(AliasMap);
//     }
//     for (auto Load: all_loads){
//         if (!Load->isSimple()) continue;
//         Hint = determineHint(Load,
//                              all_stores,
//                              all_calls,
//                              LAIInstances,
//                              VersionPairs,
//                              DI,
//                              AR.SE,
//                              AR.AA,
//                              AR.LI,
//                              AliasMap,
//                              withInstrumentation);
//         if(Hint == AliasHint::PredictNone) {
//             AAMDNodes AAInfo = Load->getAAMetadata();
//             AAInfo.PND = MDNode::get(Ctx, ArrayRef<Metadata*>());
//             Load->setAAMetadata(AAInfo);
//         }
//         else if (Hint == AliasHint::Unchanged && Load->getAAMetadata().PND) {
//             //We found that a load labelled on a previous invocation of the pass
//             //shouldn't be labelled thanks to more available information
//             AAMDNodes AAInfo = Load->getAAMetadata();
//             AAInfo.PND = nullptr;
//             Load->setAAMetadata(AAInfo);
//         }
//     }

//     for (auto LAI: LAIInstances){
//         delete LAI.second;
//     }

// }

// //TODO: replace with either IndirectUnsafe or dep.isScalar
// bool isVariantAtAllLevels(Value *Ptr, Loop *ParentLoop, LoopInfo &LI, ScalarEvolution &SE){
//     Loop *L = ParentLoop;
//     Value *NextValue = Ptr;
//     Value *PreviousValue = nullptr;
//     while ((L = L->getParentLoop())) {
//         bool nest_level_is_variant = false;
//         for (; !nest_level_is_variant; NextValue = getUnderlyingObject(NextValue, 1)){
//             //getUnderlyingObject is giving up on something that isn't a load so we're giving up too
//             if (NextValue == PreviousValue) return false;
//             PreviousValue = NextValue;

//             //we haven't worked through the whole nest yet but the underlying
//             //value is already a function argument or global variable, must be invariant
//             if (!isa<Instruction>(NextValue)) return false;

//             Instruction *NextInstruction = reinterpret_cast<Instruction *>(NextValue);

//             //found instruction on the ptr at this specific loop, check if its invariant
//             //if it is, keep stripping back instructions
//             if (L->contains(NextInstruction) && L == LI.getLoopFor(NextInstruction->getParent())){
//                 const SCEV *Scev = SE.getSCEV(NextInstruction);
//                 if (!SE.isLoopInvariant(Scev, L)) nest_level_is_variant = true;
//             }
//             //found instruction outside this level before we found any instructions within it,
//             //pointer is invariant w.r.t this level
//             else if (!L->contains(NextInstruction)) return false;

//             if (auto *Load = dyn_cast<LoadInst>(NextValue)) NextValue = Load->getPointerOperand();
//         }
//     }
//     return true;
// }

// static bool canComputePointerDiff(ScalarEvolution &SE,
//                                   const SCEV *A, const SCEV *B) {
//     if (SE.getEffectiveSCEVType(A->getType()) !=
//         SE.getEffectiveSCEVType(B->getType()))
//     return false;

//     return SE.instructionCouldExistWithOperands(A, B);
// }

// //FIXME
// bool isSeparateCacheLine(Instruction *Load, Instruction *Store, ScalarEvolution &SE, AAResults &AA){

//     LoadInst *L;
//     StoreInst *S;
//     if (isa<LoadInst>(Load)) L = reinterpret_cast<LoadInst *>(Load);
//     else { errs() << "Load argument not a load!\n"; exit(1); }
//     if (isa<StoreInst>(Store)) S = reinterpret_cast<StoreInst *>(Store);
//     else { errs() << "Store argument not a store!\n"; exit(1); }

//     MemoryLocation LocA = MemoryLocation(
//         L->getPointerOperand(),
//         LocationSize::precise(CACHE_LINE_SIZE),
//         L->getAAMetadata());
//     MemoryLocation LocB = MemoryLocation(
//         S->getPointerOperand(),
//         LocationSize::precise(CACHE_LINE_SIZE),
//         S->getAAMetadata());

//     if (AA.isNoAlias(LocA, LocB)) return true;

//     //kind of insane, but just copy pasting SCEV AA here because I don't want to figure out how to
//     //plug into it in a nice way and not sure how to enable it from clang
//     if (LocA.Size.isZero() || LocB.Size.isZero())
//     return true;

//     // This is SCEVAAResult. Get the SCEVs!
//     const SCEV *AS = SE.getSCEV(const_cast<Value *>(LocA.Ptr));
//     const SCEV *BS = SE.getSCEV(const_cast<Value *>(LocB.Ptr));

//     // If they evaluate to the same expression, it's a MustAlias.
//     if (AS == BS)
//     return false;

//     // If something is known about the difference between the two addresses,
//     // see if it's enough to prove a NoAlias.
//     if (canComputePointerDiff(SE, AS, BS)) {
//     unsigned BitWidth = SE.getTypeSizeInBits(AS->getType());
//     APInt ASizeInt(BitWidth, LocA.Size.hasValue()
//                                     ? LocA.Size.getValue()
//                                     : MemoryLocation::UnknownSize);
//     APInt BSizeInt(BitWidth, LocB.Size.hasValue()
//                                     ? LocB.Size.getValue()
//                                     : MemoryLocation::UnknownSize);

//     // Compute the difference between the two pointers.
//     const SCEV *BA = SE.getMinusSCEV(BS, AS);

//     // Test whether the difference is known to be great enough that memory of
//     // the given sizes don't overlap. This assumes that ASizeInt and BSizeInt
//     // are non-zero, which is special-cased above.
//     if (!isa<SCEVCouldNotCompute>(BA) &&
//         ASizeInt.ule(SE.getUnsignedRange(BA).getUnsignedMin()) &&
//         (-BSizeInt).uge(SE.getUnsignedRange(BA).getUnsignedMax()))
//         return true;

//     // Folding the subtraction while preserving range information can be tricky
//     // (because of INT_MIN, etc.); if the prior test failed, swap AS and BS
//     // and try again to see if things fold better that way.

//     // Compute the difference between the two pointers.
//     const SCEV *AB = SE.getMinusSCEV(AS, BS);

//     // Test whether the difference is known to be great enough that memory of
//     // the given sizes don't overlap. This assumes that ASizeInt and BSizeInt
//     // are non-zero, which is special-cased above.
//     if (!isa<SCEVCouldNotCompute>(AB) &&
//         BSizeInt.ule(SE.getUnsignedRange(AB).getUnsignedMin()) &&
//         (-ASizeInt).uge(SE.getUnsignedRange(AB).getUnsignedMax()))
//         return true;
//     }

//     return false;
// }

// unsigned checkInstrumentationInfo(LoadInst *Load, StoreInst *Store, AliasMapType& AliasMap, unsigned long long threshold){
//     const Function *F1 = Load->getFunction();
//     const Function *F2 = Store->getFunction();
//     // Checking if load and store in same function. This actually doesnt change anything
//     // Should remove this after verifing
//     if (F1!=F2){
//         return false;
//     }
    
//     // Get the debug locations
//     const DebugLoc &LDL = Load->getDebugLoc();
//     const DebugLoc &SDL = Store->getDebugLoc();

//     // Check if both locations are valid
//     if (LDL && SDL) {
//         std::string loadLoc = std::to_string(LDL.getLine()) + ":" + std::to_string(LDL.getCol());
//         std::string storeLoc = std::to_string(SDL.getLine()) + ":" + std::to_string(SDL.getCol());
//         if (DILocation *Loc = LDL.get()) {
//             // Get the scope (DIScope) of the location
//             DIScope *Scope = Loc->getScope();
//             // Retrieve the file name and directory
//             if (DIFile *File = Scope->getFile()) {
//                 std::string FileLoc = File->getFilename().str();
                
//                 // Check alias here:
//                 if (AliasMap[FileLoc][loadLoc].find(storeLoc) != AliasMap[FileLoc][loadLoc].end()){
//                     // ToDo:
//                     // Why are we checking only percentage
//                     // if ((AliasMap[FileLoc][loadLoc][storeLoc]*100)/AliasMap[FileLoc][loadLoc]["execCount"] > threshold){
//                     //     errs() << "\nSUCCESS.. Caught May Alias from being marked PND\n";
//                     //     errs() << loadLoc << " " << storeLoc << " - " << AliasMap[FileLoc][loadLoc]["execCount"] << "\n";
//                     //     errs() << (AliasMap[FileLoc][loadLoc][storeLoc]*100)/AliasMap[FileLoc][loadLoc]["execCount"] << "\n";
//                     //     return true;
//                     // }
//                     return AliasMap[FileLoc][loadLoc][storeLoc];
//                 }
//             }
//         }
//     }
//     return 0;
// }

// AliasHint AliasHintsPass::determineHint(LoadInst *Load, SmallVector<StoreInst *> all_stores,
//                                         SmallVector<CallInst *> all_calls, std::map<Loop *, LoopAccessInfo *> LAIInstances, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, DependenceInfo DI, ScalarEvolution &SE,
//                                         AAResults &AA, LoopInfo &LI, AliasMapType &AliasMap, bool withInstrumentation){
//     std::unique_ptr<Dependence> Dep;
//     Loop *current_loop = LI.getLoopFor(Load->getParent());
//     std::map<Instruction *, MemoryDepChecker::Dependence::DepType> store_map;
//     bool was_LAI_analysed = false;
//     if (current_loop->isInnermost()){
//         LoopAccessInfo *LAI = LAIInstances[current_loop];
//         MemoryDepChecker MDC = LAI->getDepChecker();
//         if (MDC.QueryResults.find(Load) != MDC.QueryResults.end()){
//             was_LAI_analysed = true;
//             store_map = MDC.QueryResults[Load];
//         }
//     }

//     // Start with PredictNone
//     AliasHint label = AliasHint::PredictNone;

//     for (auto Store: all_stores){
//         if (!withinSameVersion(Load, Store, VersionPairs, LI)) continue;
//         if (withInstrumentation){
//             // Do this if using instrumentation
//             auto AAresult = AA.alias(Store->getPointerOperand(), Load->getPointerOperand());
//             if (AAresult==AliasResult::NoAlias) continue;
//             // if (AAresult==AliasResult::MayAlias && !checkInstrumentationInfo(Load, Store, AliasMap, threshold)) continue;
//         }
//         else{
//             // Do this if not using instumentation
//             if (!AA.isMustAlias(Store->getPointerOperand(), Load->getPointerOperand())) continue;
//             // if (AA.alias(Store->getPointerOperand(), Load->getPointerOperand())==AliasResult::NoAlias) continue;
//         }
//         Dep = DI.depends(Store, Load, true);
//         if (!Dep) continue;
//         if (!isProblematicDep(Load, Dep.get(), LI, SE, AA)) continue;
//         if (was_LAI_analysed && LI.getLoopFor(Store->getParent()) == current_loop &&
//             Store->isSimple() && store_map.find(Store) != store_map.end()){
//             MemoryDepChecker::Dependence::DepType Type = store_map[Store];
//             switch (Type){
//                 case MemoryDepChecker::Dependence::NoDep:
//                 case MemoryDepChecker::Dependence::IndependentStride:
//                 case MemoryDepChecker::Dependence::SafeDistance: //maybe need to do forward checks?
//                     continue;
//                 case MemoryDepChecker::Dependence::Forward:
//                 case MemoryDepChecker::Dependence::ForwardButPreventsForwarding:
//                     if (isVariantAtAllLevels(Load->getPointerOperand(), current_loop, LI, SE) &&
//                         isSeparateCacheLine(Load, Store, SE, AA))
//                         continue;
//                     label = AliasHint::Unchanged;
//                     break;
//                 default:
//                     label =  AliasHint::Unchanged;
//                     break;
//             }
//         }
//         else {
//             label = AliasHint::Unchanged;
//         }
//     }


//     // Threshold for when to not mark PND
//     unsigned long long threshold = 5;

//     unsigned aliascount = 0;
//     if (withInstrumentation && label==AliasHint::Unchanged){
//         for (auto Store: all_stores){
//                 auto AAresult = AA.alias(Store->getPointerOperand(), Load->getPointerOperand());
//                 if (AAresult==AliasResult::MustAlias) return label;
//                 if (AAresult==AliasResult::MayAlias) aliascount+=checkInstrumentationInfo(Load, Store, AliasMap, threshold);
//         }

//         // Get debug info
//         const DebugLoc &LDL = Load->getDebugLoc();
//         if (LDL) {
//             std::string loadLoc = std::to_string(LDL.getLine()) + ":" + std::to_string(LDL.getCol());
//             if (DILocation *Loc = LDL.get()) {
//                 // Get the scope (DIScope) of the location
//                 DIScope *Scope = Loc->getScope();
//                 // Retrieve the file name and directory
//                 if (DIFile *File = Scope->getFile()) {
//                     std::string FileLoc = File->getFilename().str();
//                     if (AliasMap[FileLoc].find(loadLoc) != AliasMap[FileLoc].end() && AliasMap[FileLoc][loadLoc]["execCount"]>0){
//                         if (aliascount*100/AliasMap[FileLoc][loadLoc]["execCount"] < threshold){
//                             errs() << "Marking uncaught PND.." << "\n";
//                             errs() << aliascount << "  " << AliasMap[FileLoc][loadLoc]["execCount"] << "  " << aliascount*100/AliasMap[FileLoc][loadLoc]["execCount"] << "\n";
//                             label = AliasHint::PredictNone;
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     for (auto Call: all_calls){
//         if (!withinSameVersion(Load, Call, VersionPairs, LI)) continue;
//         ModRefInfo res = AA.getModRefInfo(Load, Call);
//         if (isModSet(res)) return AliasHint::Unchanged;
//     }
    
//     return label;
// }

// bool AliasHintsPass::isProblematicDep(LoadInst *Load, Dependence *Dep, LoopInfo &LI, ScalarEvolution &SE, AAResults &AA){
//     if (Dep->isConfused()) return true;
//     if (Dep->isOrdered()){
//         Loop *LoadLoop = LI.getLoopFor(Load->getParent());
//         unsigned LoadDepth = LoadLoop->getLoopDepth();
//         Instruction *Source = Dep->getSrc();
//         StoreInst *Store = reinterpret_cast<StoreInst *>(Source);
//         unsigned StoreDepth = LI.getLoopFor(Store->getParent())->getLoopDepth();
//         Dep->normalize(&SE);
//         unsigned Levels = Dep->getLevels();
//         if ((Dep->isPeelFirst(LoadDepth) || Dep->isPeelLast(LoadDepth)) && LoadLoop->isOutermost()) return false;
//         //if cross-loop dep we can reason about instruction distances. this assumes loops are generally large and small kernels.
//         if (LoadDepth != StoreDepth){
//             int Diff = std::abs((int)LoadDepth - (int)StoreDepth);
//             if (Diff == 1) return true;
//             if (Diff == 2){
//                 if (Levels == std::min(LoadDepth, StoreDepth)) return true;
//                 return false;
//             }
//             if (Diff > 2) return false;
//         }
//         //if within the same loop it is simpler to reason about dep distances
//         const SCEV *scev_distance;
//         if (Levels > 2){
//             //does there exist a distance greater than one at a level greater than two nests above the dep?
//             for (unsigned Level=1; Level < Levels-1; Level++){
//                 uint64_t distance;
//                 scev_distance = Dep->getDistance(Level);
//                 if (!scev_distance) continue;
//                 distance = SE.getUnsignedRangeMin(scev_distance).getZExtValue();
//                 if (Levels - Level > 2 && distance >= 1) return false;
//                 if (distance >= 2) return false;
//             }
//         }
//         return true;
//     }
//     //dep is unordered
//     return false;
// }

// SmallVector<std::pair<Loop *, Loop *>, 2> AliasHintsPass::findVersionedLoops(LoopNest &LN, SmallVector<BasicBlock *, 1> GeneratedChecks, LoopInfo &LI, DominatorTree &DT){

//     SmallVector<std::pair<Loop *, Loop *>, 2> Result;

//     for (auto MemCheckBlock: GeneratedChecks){
//         Loop *L0 = LI.getLoopFor(MemCheckBlock);
//         if (!L0) continue;
//         unsigned Depth = L0->getLoopDepth() + 1;
//         Loop *L1, *L2;
//         BranchInst *CondBr = reinterpret_cast<BranchInst *>(MemCheckBlock->getTerminator());
//         BasicBlock *B1 = CondBr->getSuccessor(0);
//         BasicBlock *B2 = CondBr->getSuccessor(1);
//         LoopVectorTy Loops = LN.getLoopsAtDepth(Depth);
//         for (auto Loop: Loops){
//             BasicBlock *Preheader = Loop->getLoopPreheader();
//             if (DT.dominates(B1, Preheader)) L1 = Loop;
//             else if (DT.dominates(B2, Preheader)) L2 = Loop;
//         }
//         Result.push_back(std::pair<Loop *, Loop*>(L1, L2));
//     }

//     return Result;
// }

// //For a pair of loops separated by a vectorising runtime check, are a given load and given store/call dominated by the same one? if not, they exist on opposite sides of the runtime check and shouldn't be compared
// bool AliasHintsPass::withinSameVersion(LoadInst *Load, Instruction *DepInst, SmallVector<std::pair<Loop *, Loop *>, 2> VersionPairs, LoopInfo &LI){
//     Loop *L1 = LI.getLoopFor(Load->getParent());
//     Loop *L2 = LI.getLoopFor(DepInst->getParent());

//     for (auto VersionPair: VersionPairs)
//         if ((VersionPair.first->contains(L1) && VersionPair.second->contains(L2))
//             || (VersionPair.first->contains(L2) && VersionPair.second->contains(L1)))
//             return false;

//     return true;
// }

// llvm::PassPluginLibraryInfo getAliasHintsPassPluginInfo() {
//   return {LLVM_PLUGIN_API_VERSION, "AliasHints", LLVM_VERSION_STRING,
//           [](PassBuilder &PB) {
//             PB.registerLateLoopOptimizationsEPCallback(
//                 [](llvm::LoopPassManager &LPM, OptimizationLevel Level) {
//                   LPM.addPass(AliasHintsPass());
//                 });
//             PB.registerPipelineParsingCallback(
//                 [](StringRef Name, llvm::LoopPassManager &LPM,
//                    ArrayRef<llvm::PassBuilder::PipelineElement>) {
//                   if (Name == "aliashints") {
//                     LPM.addPass(AliasHintsPass());
//                     return true;
//                   }
//                   return false;
//                 });
//           }};
// }

// #ifndef LLVM_ALIASHINTSPASS_LINK_INTO_TOOLS
// extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
// llvmGetPassPluginInfo() {
//   return getAliasHintsPassPluginInfo();
// }
// #endif
