#ifndef LLVM_TRANSFORMS_ALIASHINTS_INSTRUMENTATION_H
#define LLVM_TRANSFORMS_ALIASHINTS_INSTRUMENTATION_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class AliasHintsInstrumentationPass : public PassInfoMixin<AliasHintsInstrumentationPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MM);
  bool runOnModule(Module &M);
};

}

#endif