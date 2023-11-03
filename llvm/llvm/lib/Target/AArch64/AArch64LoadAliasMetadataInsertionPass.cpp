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
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPApost; //LDRSBXpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApost; //LDRSBXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPApre; //LDRSBXpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApre; //LDRSBXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPAui; //LDRSBXui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAui; //LDRSBXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPAroW; //LDRSBXroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroW; //LDRSBXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPAroX; //LDRSBXroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroX; //LDRSBXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPApost; //LDRSBWpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApost; //LDRSBWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPApre; //LDRSBWpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApre; //LDRSBWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPAui; //LDRSBWui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAui; //LDRSBWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPAroW; //LDRSBWroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroW; //LDRSBWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPAroX; //LDRSBWroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroX; //LDRSBWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPApost; //LDRSHXpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApost; //LDRSHXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPApre; //LDRSHXpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApre; //LDRSHXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPAui; //LDRSHXui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAui; //LDRSHXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPAroW; //LDRSHXroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroW; //LDRSHXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPAroX; //LDRSHXroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroX; //LDRSHXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPApost; //LDRSHWpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApost; //LDRSHWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPApre; //LDRSHWpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApre; //LDRSHWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPAui; //LDRSHWui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAui; //LDRSHWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPAroW; //LDRSHWroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroW; //LDRSHWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPAroX; //LDRSHWroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroX; //LDRSHWroX-No-Predict-Alias
        else continue;
        break;

      case AArch64::LDURQi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURQPAi; //LDURQi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURQPNAi; //LDURQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURDi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURDPAi; //LDURDi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURDPNAi; //LDURDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURXi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURXPAi; //LDURXi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURXPNAi; //LDURXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSPAi; //LDURSi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSPNAi; //LDURSi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSWi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSWPAi; //LDURSWi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSWPNAi; //LDURSWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURWi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURWPAi; //LDURWi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURWPNAi; //LDURWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURHHi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURHHPAi; //LDURHHi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURHHPNAi; //LDURHHi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURBBi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURBBPAi; //LDURBBi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURBBPNAi; //LDURBBi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPAi; //LDPQi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNAi; //LDPQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPApost; //LDPQpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApost; //LDPQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPApre; //LDPQpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApre; //LDPQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPAi; //LDPDi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNAi; //LDPDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPApost; //LDPDpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApost; //LDPDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPApre; //LDPDpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApre; //LDPDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPAi; //LDPXi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNAi; //LDPXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPApost; //LDPXpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApost; //LDPXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPApre; //LDPXpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApre; //LDPXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWi:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPAi; //LDPWi-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNAi; //LDPWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPApost; //LDPWpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApost; //LDPWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPApre; //LDPWpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApre; //LDPWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPApost; //LDRQpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApost; //LDRQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPApre; //LDRQpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApre; //LDRQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPAui; //LDRQui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAui; //LDRQui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPAroW; //LDRQroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroW; //LDRQroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPAroX; //LDRQroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroX; //LDRQroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPApost; //LDRDpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApost; //LDRDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPApre; //LDRDpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApre; //LDRDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPAui; //LDRDui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAui; //LDRDui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPAroW; //LDRDroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroW; //LDRDroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPAroX; //LDRDroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroX; //LDRDroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPApost; //LDRSpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApost; //LDRSpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPApre; //LDRSpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApre; //LDRSpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPAui; //LDRSui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAui; //LDRSui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPAroW; //LDRSroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroW; //LDRSroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPAroX; //LDRSroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroX; //LDRSroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPApost; //LDRWpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApost; //LDRWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPApre; //LDRWpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApre; //LDRWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPAui; //LDRWui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAui; //LDRWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPAroW; //LDRWroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroW; //LDRWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPAroX; //LDRWroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroX; //LDRWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPApost; //LDRXpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApost; //LDRXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPApre; //LDRXpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApre; //LDRXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPAui; //LDRXui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAui; //LDRXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPAroW; //LDRXroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroW; //LDRXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPAroX; //LDRXroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroX; //LDRXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPApost; //LDRSWpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApost; //LDRSWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPApre; //LDRSWpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApre; //LDRSWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPAui; //LDRSWui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAui; //LDRSWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPAroW; //LDRSWroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroW; //LDRSWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPAroX; //LDRSWroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroX; //LDRSWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPApost; //LDRHHpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApost; //LDRHHpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPApre; //LDRHHpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApre; //LDRHHpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPAui; //LDRHHui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAui; //LDRHHui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPAroW; //LDRHHroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroW; //LDRHHroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPAroX; //LDRHHroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroX; //LDRHHroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpost:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPApost; //LDRBBpost-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApost; //LDRBBpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpre:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPApre; //LDRBBpre-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApre; //LDRBBpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBui:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPAui; //LDRBBui-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAui; //LDRBBui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroW:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPAroW; //LDRBBroW-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAroW; //LDRBBroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroX:
        if (hasHint(MI, PREDICT_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPAroX; //LDRBBroX-Predict-Alias
        else if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
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
