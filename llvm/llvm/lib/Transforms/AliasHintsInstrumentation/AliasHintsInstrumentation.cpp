//========================================================================
// FILE:
//    AliasHintsInstrumentation.cpp
//
// DESCRIPTION:
//
// USAGE:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libAliasHintsInstrumentation.so `\`
//        -passes=-"aliashint-instrument" <bitcode-file> -o instrumentend.bin
//      $ lli instrumented.bin
//
// License: MIT
//========================================================================
#include "llvm/Transforms/AliasHintsInstrumentation/AliasHintsInstrumentation.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <list>
#include <iostream>
#include <cstdio>
#include <stdio.h>

using namespace llvm;

#define DEBUG_TYPE "aliashint-instrument"

//-----------------------------------------------------------------------------
// AliasHintsInstrumentation implementation
//-----------------------------------------------------------------------------
bool AliasHintsInstrumentationPass::runOnModule(Module &M) {

    auto &CTX = M.getContext();


    // STEP 1: Inject the declaration of printf
    // ----------------------------------------
    // Create (or _get_ in cases where it's already available) the following
    // declaration in the IR module:
    //    declare i32 @printf(i8*, ...)
    // It corresponds to the following C declaration:
    //    int printf(char *, ...)
    PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));
    FunctionType *PrintfTy =
        FunctionType::get(IntegerType::getInt32Ty(CTX), PrintfArgTy,
                            /*IsVarArgs=*/true);

    FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

    // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
    Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
    PrintfF->setDoesNotThrow();
    PrintfF->addParamAttr(0, Attribute::NoCapture);
    PrintfF->addParamAttr(0, Attribute::ReadOnly);


    // STEP 2: Inject a global variable that will hold the printf format string
    // ------------------------------------------------------------------------
    
    // Prepare printing format
    llvm::Constant *ResultFormatStr = llvm::ConstantDataArray::getString(CTX, "%s %p\n");

    // Inject the Format String
    Constant *ResultFormatStrVar = M.getOrInsertGlobal("ResultFormatStrIR", ResultFormatStr->getType());
    dyn_cast<GlobalVariable>(ResultFormatStrVar)->setInitializer(ResultFormatStr);
    dyn_cast<GlobalVariable>(ResultFormatStrVar)->setLinkage(GlobalValue::LinkOnceODRLinkage);

    IRBuilder<> Builder(CTX);
    //PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));
    llvm::Value *ResultFormatStrPtr = Builder.CreatePointerCast(ResultFormatStrVar, PrintfArgTy);


    // STEP 3: For each load/store instruction, inject instrumentation code
    // --------------------------------------------------------------------

    for (Function &F : M) {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                // Check if the instruction is a LoadInst
                if (auto *Load = dyn_cast<LoadInst>(&I)) {
                    // Process the Load instruction
                    // errs() << "(aliashint-instrumentation) Found Load " << LoadNum << ": " << *Load << "\n";

                    // Get an IR builder. Sets the insertion point just before Load
                    IRBuilder<> Builder(I.getContext());
                    // Set the insertion point just before the Load instruction
                    Builder.SetInsertPoint(Load);

                    // Get address of Load Instruction
                    Value* Address = Load->getPointerOperand();


                    // Assign mapping info 

                    // Create ID based on source code location
                    std::string LoadName;
                    if (const DebugLoc &DL = I.getDebugLoc()) {
                        std::string FileLoc = "";
                        if (DILocation *Loc = DL.get()) {
                            // Get the scope (DIScope) of the location
                            DIScope *Scope = Loc->getScope();

                            // Retrieve the file name and directory
                            if (DIFile *File = Scope->getFile()) {
                                FileLoc = File->getDirectory().str() + File->getFilename().str();
                                LoadName = "Load " + FileLoc + " " + std::to_string(DL.getLine()) + " " + std::to_string(DL.getCol());
                            } else {
                                LoadName = "Load Unknown";
                            }
                        }
                    }
                    else{
                        LoadName = "Load Unknown ";
                    }

                    // Create LLVM Constant out of string
                    llvm::Constant *LoadNameStr = llvm::ConstantDataArray::getString(I.getContext(), LoadName.c_str());
                    Constant *LoadNameStrVar = M.getOrInsertGlobal(LoadName, LoadNameStr->getType());
                    dyn_cast<GlobalVariable>(LoadNameStrVar)->setInitializer(LoadNameStr);
                    dyn_cast<GlobalVariable>(LoadNameStrVar)->setLinkage(GlobalValue::LinkOnceODRLinkage);

                    // Create Printf argument pointer for the string
                    llvm::Value *LoadNameStrPtr = Builder.CreatePointerCast(LoadNameStrVar, PrintfArgTy);
                    
                    // Create call to print information
                    Builder.CreateCall(Printf, {ResultFormatStrPtr, LoadNameStrPtr, Address});

                }
                // Check if the instruction is a StoreInst
                else if (auto *Store = dyn_cast<StoreInst>(&I)) {
                    // Process the Store instruction
                    // errs() << "(aliashint-instrumentation) Found Store " << StoreNum << ": " << *Store << "\n";

                    // Get an IR builder. Sets the insertion point just before Store
                    IRBuilder<> Builder(I.getContext());
                    // Set the insertion point just before the Store instruction
                    Builder.SetInsertPoint(Store);

                    // Get address of Store Instruction
                    Value* Address = Store->getOperand(1);


                    // Assign mapping info 

                    // Read Unique Id if the PNDUniqueIds pass was run
                    std::string StoreName;
                    if (const DebugLoc &DL = I.getDebugLoc()) {
                        std::string FileLoc = "";
                        if (DILocation *Loc = DL.get()) {
                            // Get the scope (DIScope) of the location
                            DIScope *Scope = Loc->getScope();

                            // Retrieve the file name and directory
                            if (DIFile *File = Scope->getFile()) {
                                FileLoc = File->getDirectory().str() + File->getFilename().str();
                                StoreName = "Store " + FileLoc + " " + std::to_string(DL.getLine()) + " " + std::to_string(DL.getCol());
                            } else {
                                StoreName = "Store Unknown";
                            }
                        }
                    }
                    else{
                        StoreName = "Store Unknown";
                    }
                    // Create LLVM Constant out of string
                    llvm::Constant *StoreNameStr = llvm::ConstantDataArray::getString(I.getContext(), StoreName.c_str());
                    Constant *StoreNameStrVar = M.getOrInsertGlobal(StoreName, StoreNameStr->getType());
                    dyn_cast<GlobalVariable>(StoreNameStrVar)->setInitializer(StoreNameStr);
                    dyn_cast<GlobalVariable>(StoreNameStrVar)->setLinkage(GlobalValue::LinkOnceODRLinkage);

                    // Create Printf argument pointer for the string
                    llvm::Value *StoreNameStrPtr = Builder.CreatePointerCast(StoreNameStrVar, PrintfArgTy);

                    // Create call to print information
                    Builder.CreateCall(Printf, {ResultFormatStrPtr, StoreNameStrPtr, Address});
                }
            }
        }
    }


  return true;
}

PreservedAnalyses AliasHintsInstrumentationPass::run(llvm::Module &M,
                                          llvm::ModuleAnalysisManager &MM) {
  bool Changed = runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getAliasHintsInstrumentationPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "aliashint-instrument", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "aliashint-instrument") {
                    MPM.addPass(AliasHintsInstrumentationPass());
                    return true;
                  }
                  return false;
                });
          }};
}

#ifndef LLVM_ALIASHINTSINSTRUMENTATIONPASS_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getAliasHintsInstrumentationPassPluginInfo();
}
#endif