//==-- AArch64LoadAliasMetadataInsertion.cpp - Replace dead defs w/ zero reg --==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// Scan for IR level metadata on MemOperand's to find load opcodes that should
/// be replaced with an alias metadata load opcode.
//===----------------------------------------------------------------------===//

#include "AArch64.h"
#include "AArch64RegisterInfo.h"
#include "AArch64Subtarget.h"
#include "AArch64InstrInfo.h"
#include "AArch64MachineFunctionInfo.h"
#include "AArch64Subtarget.h"
#include "MCTargetDesc/AArch64MCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "aarch64-alias-loads"

STATISTIC(NumOpcodesReplaced, "Number of load opcodes replaced");

#define AARCH64_LOAD_ALIAS_METADATA_NAME "AArch64 Load alias metadata"


namespace {
class AArch64LoadAliasMetadataInsertion : public MachineFunctionPass {
private:
  const TargetRegisterInfo *TRI;
  const MachineRegisterInfo *MRI;
  const AArch64InstrInfo *TII;
  bool Changed;
  void processMachineBasicBlock(MachineBasicBlock &MBB);
public:
  static char ID; // Pass identification, replacement for typeid.
  AArch64LoadAliasMetadataInsertion() : MachineFunctionPass(ID) {
    initializeAArch64LoadAliasMetadataInsertionPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &F) override;

  StringRef getPassName() const override { return AARCH64_LOAD_ALIAS_METADATA_NAME; }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
};
char AArch64LoadAliasMetadataInsertion::ID = 0;
} // end anonymous namespace

INITIALIZE_PASS(AArch64LoadAliasMetadataInsertion, "aarch64-load-alias-metadata",
                AARCH64_LOAD_ALIAS_METADATA_NAME, false, false)

bool hasHint(MachineInstr &MI, unsigned AddrSpace){
  return false;
  for (auto MemOp: MI.memoperands()){
    const Value *ir_pointer = MemOp->getValue();
    if (ir_pointer == NULL)
      continue;
    if (ir_pointer->getType()->isPointerTy())
      if (ir_pointer->getType()->getPointerAddressSpace() == AddrSpace){
        return true;
      }
  }
  return false;
}

void AArch64LoadAliasMetadataInsertion::processMachineBasicBlock(
    MachineBasicBlock &MBB) {
  DebugLoc DL;
  unsigned NewOpcode;
  for (MachineInstr &MI: MBB){
    switch (MI.getOpcode()){
      case AArch64::LDRSBXpost:

          NewOpcode = AArch64::LDRSBXPApost; //LDRSBXpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApost; //LDRSBXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXpre:

          NewOpcode = AArch64::LDRSBXPApre; //LDRSBXpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApre; //LDRSBXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXui:

          NewOpcode = AArch64::LDRSBXPAui; //LDRSBXui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAui; //LDRSBXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroW:

          NewOpcode = AArch64::LDRSBXPAroW; //LDRSBXroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroW; //LDRSBXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroX:

          NewOpcode = AArch64::LDRSBXPAroX; //LDRSBXroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroX; //LDRSBXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpost:

          NewOpcode = AArch64::LDRSBWPApost; //LDRSBWpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApost; //LDRSBWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpre:

          NewOpcode = AArch64::LDRSBWPApre; //LDRSBWpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApre; //LDRSBWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWui:

          NewOpcode = AArch64::LDRSBWPAui; //LDRSBWui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAui; //LDRSBWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroW:

          NewOpcode = AArch64::LDRSBWPAroW; //LDRSBWroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroW; //LDRSBWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroX:

          NewOpcode = AArch64::LDRSBWPAroX; //LDRSBWroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroX; //LDRSBWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpost:

          NewOpcode = AArch64::LDRSHXPApost; //LDRSHXpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApost; //LDRSHXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpre:

          NewOpcode = AArch64::LDRSHXPApre; //LDRSHXpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApre; //LDRSHXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXui:

          NewOpcode = AArch64::LDRSHXPAui; //LDRSHXui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAui; //LDRSHXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroW:

          NewOpcode = AArch64::LDRSHXPAroW; //LDRSHXroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroW; //LDRSHXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroX:

          NewOpcode = AArch64::LDRSHXPAroX; //LDRSHXroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroX; //LDRSHXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpost:

          NewOpcode = AArch64::LDRSHWPApost; //LDRSHWpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApost; //LDRSHWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpre:

          NewOpcode = AArch64::LDRSHWPApre; //LDRSHWpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApre; //LDRSHWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWui:

          NewOpcode = AArch64::LDRSHWPAui; //LDRSHWui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAui; //LDRSHWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroW:

          NewOpcode = AArch64::LDRSHWPAroW; //LDRSHWroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroW; //LDRSHWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroX:

          NewOpcode = AArch64::LDRSHWPAroX; //LDRSHWroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroX; //LDRSHWroX-No-Predict-Alias
        else continue;
        break;

      case AArch64::LDURQi:

          NewOpcode = AArch64::LDURQPAi; //LDURQi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURQPNAi; //LDURQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURDi:

          NewOpcode = AArch64::LDURDPAi; //LDURDi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURDPNAi; //LDURDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURXi:

          NewOpcode = AArch64::LDURXPAi; //LDURXi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURXPNAi; //LDURXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSi:

          NewOpcode = AArch64::LDURSPAi; //LDURSi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSPNAi; //LDURSi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSWi:

          NewOpcode = AArch64::LDURSWPAi; //LDURSWi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSWPNAi; //LDURSWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURWi:

          NewOpcode = AArch64::LDURWPAi; //LDURWi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURWPNAi; //LDURWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURHHi:

          NewOpcode = AArch64::LDURHHPAi; //LDURHHi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURHHPNAi; //LDURHHi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURBBi:

          NewOpcode = AArch64::LDURBBPAi; //LDURBBi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURBBPNAi; //LDURBBi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQi:

          NewOpcode = AArch64::LDPQPAi; //LDPQi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNAi; //LDPQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpost:

          NewOpcode = AArch64::LDPQPApost; //LDPQpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApost; //LDPQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpre:

          NewOpcode = AArch64::LDPQPApre; //LDPQpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApre; //LDPQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDi:

          NewOpcode = AArch64::LDPDPAi; //LDPDi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNAi; //LDPDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpost:

          NewOpcode = AArch64::LDPDPApost; //LDPDpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApost; //LDPDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpre:

          NewOpcode = AArch64::LDPDPApre; //LDPDpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApre; //LDPDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXi:

          NewOpcode = AArch64::LDPXPAi; //LDPXi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNAi; //LDPXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpost:

          NewOpcode = AArch64::LDPXPApost; //LDPXpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApost; //LDPXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpre:

          NewOpcode = AArch64::LDPXPApre; //LDPXpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApre; //LDPXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWi:

          NewOpcode = AArch64::LDPWPAi; //LDPWi-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNAi; //LDPWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpost:

          NewOpcode = AArch64::LDPWPApost; //LDPWpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApost; //LDPWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpre:

          NewOpcode = AArch64::LDPWPApre; //LDPWpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApre; //LDPWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpost:

          NewOpcode = AArch64::LDRQPApost; //LDRQpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApost; //LDRQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpre:

          NewOpcode = AArch64::LDRQPApre; //LDRQpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApre; //LDRQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQui:

          NewOpcode = AArch64::LDRQPAui; //LDRQui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAui; //LDRQui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroW:

          NewOpcode = AArch64::LDRQPAroW; //LDRQroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroW; //LDRQroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroX:

          NewOpcode = AArch64::LDRQPAroX; //LDRQroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroX; //LDRQroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpost:

          NewOpcode = AArch64::LDRDPApost; //LDRDpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApost; //LDRDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpre:

          NewOpcode = AArch64::LDRDPApre; //LDRDpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApre; //LDRDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDui:

          NewOpcode = AArch64::LDRDPAui; //LDRDui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAui; //LDRDui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroW:

          NewOpcode = AArch64::LDRDPAroW; //LDRDroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroW; //LDRDroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroX:

          NewOpcode = AArch64::LDRDPAroX; //LDRDroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroX; //LDRDroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpost:

          NewOpcode = AArch64::LDRSPApost; //LDRSpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApost; //LDRSpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpre:

          NewOpcode = AArch64::LDRSPApre; //LDRSpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApre; //LDRSpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSui:

          NewOpcode = AArch64::LDRSPAui; //LDRSui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAui; //LDRSui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroW:

          NewOpcode = AArch64::LDRSPAroW; //LDRSroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroW; //LDRSroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroX:

          NewOpcode = AArch64::LDRSPAroX; //LDRSroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroX; //LDRSroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpost:

          NewOpcode = AArch64::LDRWPApost; //LDRWpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApost; //LDRWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpre:

          NewOpcode = AArch64::LDRWPApre; //LDRWpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApre; //LDRWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWui:

          NewOpcode = AArch64::LDRWPAui; //LDRWui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAui; //LDRWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroW:

          NewOpcode = AArch64::LDRWPAroW; //LDRWroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroW; //LDRWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroX:

          NewOpcode = AArch64::LDRWPAroX; //LDRWroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroX; //LDRWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpost:

          NewOpcode = AArch64::LDRXPApost; //LDRXpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApost; //LDRXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpre:

          NewOpcode = AArch64::LDRXPApre; //LDRXpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApre; //LDRXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXui:

          NewOpcode = AArch64::LDRXPAui; //LDRXui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAui; //LDRXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroW:

          NewOpcode = AArch64::LDRXPAroW; //LDRXroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroW; //LDRXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroX:

          NewOpcode = AArch64::LDRXPAroX; //LDRXroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroX; //LDRXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpost:

          NewOpcode = AArch64::LDRSWPApost; //LDRSWpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApost; //LDRSWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpre:

          NewOpcode = AArch64::LDRSWPApre; //LDRSWpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApre; //LDRSWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWui:

          NewOpcode = AArch64::LDRSWPAui; //LDRSWui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAui; //LDRSWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroW:

          NewOpcode = AArch64::LDRSWPAroW; //LDRSWroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroW; //LDRSWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroX:

          NewOpcode = AArch64::LDRSWPAroX; //LDRSWroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroX; //LDRSWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpost:

          NewOpcode = AArch64::LDRHHPApost; //LDRHHpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApost; //LDRHHpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpre:

          NewOpcode = AArch64::LDRHHPApre; //LDRHHpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApre; //LDRHHpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHui:

          NewOpcode = AArch64::LDRHHPAui; //LDRHHui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAui; //LDRHHui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroW:

          NewOpcode = AArch64::LDRHHPAroW; //LDRHHroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroW; //LDRHHroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroX:

          NewOpcode = AArch64::LDRHHPAroX; //LDRHHroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroX; //LDRHHroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpost:

          NewOpcode = AArch64::LDRBBPApost; //LDRBBpost-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApost; //LDRBBpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpre:

          NewOpcode = AArch64::LDRBBPApre; //LDRBBpre-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApre; //LDRBBpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBui:

          NewOpcode = AArch64::LDRBBPAui; //LDRBBui-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAui; //LDRBBui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroW:

          NewOpcode = AArch64::LDRBBPAroW; //LDRBBroW-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAroW; //LDRBBroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroX:

          NewOpcode = AArch64::LDRBBPAroX; //LDRBBroX-Predict-Alias
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAroX; //LDRBBroX-No-Predict-Alias
        else continue;
        break;
      default:
        if (!MI.mayStore() && (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE) || hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))){
          printf("found MI with hint but no match!\n");
          MI.print(llvm::outs());
          printf("\n");
        }
        continue;
    }
    MI.setDesc(TII->get(NewOpcode));
    NumOpcodesReplaced++;
  }
}

bool AArch64LoadAliasMetadataInsertion::runOnMachineFunction(MachineFunction &MF) {

  if (skipFunction(MF.getFunction())){
    return false;
  }

  TRI = MF.getSubtarget().getRegisterInfo();
  TII = MF.getSubtarget<AArch64Subtarget>().getInstrInfo();
  MRI = &MF.getRegInfo();
  LLVM_DEBUG(dbgs() << "***** AArch64LoadAliasMetadataInsertion *****\n");
  Changed = false;
  for (auto &MBB : MF)
    processMachineBasicBlock(MBB);
  if (NumOpcodesReplaced > 0) Changed = 1;
  return Changed;
}

FunctionPass *llvm::createAArch64LoadAliasMetadataInsertion() {
  return new AArch64LoadAliasMetadataInsertion();
}
