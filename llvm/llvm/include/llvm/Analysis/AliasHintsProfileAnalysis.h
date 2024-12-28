#ifndef LLVM_ANALYSIS_ALIASHINTSPROFILEANALYSIS_H
#define LLVM_ANALYSIS_ALIASHINTSPROFILEANALYSIS_H

#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Analysis/LoopNestAnalysis.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include <unordered_map>



namespace llvm {

typedef std::unordered_map<LoadInst*, std::unordered_map<StoreInst*, bool>> AliasMapType;

class AliasHintsProfileAnalysisPass : public PassInfoMixin<AliasHintsProfileAnalysisPass> {
public:
  //PreservedAnalyses run(Module &M, ModuleAnalysisManager &MPM);
  PreservedAnalyses run(LoopNest &LN, LoopAnalysisManager &AM, LoopStandardAnalysisResults &AR, LPMUpdater &U);
  inline void attachMetadata(LoadInst &I, const std::unordered_map<StoreInst *, bool> &Map);
};

}

#endif