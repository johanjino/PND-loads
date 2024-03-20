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
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Instruction.h"
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
  unsigned numLoads = 0;
  unsigned NumOpcodesReplaced;
  void processMachineBasicBlock(MachineBasicBlock &MBB, std::vector<unsigned> &PNDOffsets);
  bool hasHint(MachineInstr &MI, unsigned AddrSpace);
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

bool AArch64LoadAliasMetadataInsertion::hasHint(MachineInstr &MI, unsigned AddrSpace){
  numLoads++;
  for (auto MemOp: MI.memoperands()){
    AAMDNodes AAInfo = MemOp->getAAInfo();
    if (AAInfo.PND) {
      //errs() << "BACKEND COUNT: 1\n";
      return true;
    }
  }
  return false;
}

void AArch64LoadAliasMetadataInsertion::processMachineBasicBlock(
    MachineBasicBlock &MBB, std::vector<unsigned> &PNDOffsets) {
  DebugLoc DL;
  unsigned NewOpcode;
  for (MachineInstr &MI: MBB){
    switch (MI.getOpcode()){
      case AArch64::LDURQi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURQPNAi; //LDURQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURDi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURDPNAi; //LDURDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURXi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURXPNAi; //LDURXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSPNAi; //LDURSi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSBXi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSBXPNAi; //LDURSBXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSBWi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSBWPNAi; //LDURSBWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSHXi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSHXPNAi; //LDURSHXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURSWi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURSWPNAi; //LDURSWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURWi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURWPNAi; //LDURWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURHHi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURHHPNAi; //LDURHHi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDURBBi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDURBBPNAi; //LDURBBi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNAi; //LDPQi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApost; //LDPQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPQpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPQPNApre; //LDPQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNAi; //LDPDi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApost; //LDPDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPDpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPDPNApre; //LDPDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNAi; //LDPXi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApost; //LDPXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPXpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPXPNApre; //LDPXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSWi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSWPNAi; //LDPSWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSWPNApost; //LDPSWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSWPNApre; //LDPSWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSPNAi; //LDPSi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSPNApost; //LDPSpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPSpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPSPNApre; //LDPSpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWi:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNAi; //LDPWi-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApost; //LDPWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDPWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDPWPNApre; //LDPWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApost; //LDRSBXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNApre; //LDRSBXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAui; //LDRSBXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroW; //LDRSBXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBXroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBXPNAroX; //LDRSBXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApost; //LDRSBWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNApre; //LDRSBWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAui; //LDRSBWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroW; //LDRSBWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSBWroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSBWPNAroX; //LDRSBWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApost; //LDRSHXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNApre; //LDRSHXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAui; //LDRSHXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroW; //LDRSHXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHXroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHXPNAroX; //LDRSHXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApost; //LDRSHWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNApre; //LDRSHWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAui; //LDRSHWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroW; //LDRSHWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSHWroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSHWPNAroX; //LDRSHWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQl:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAl; //LDRQl-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApost; //LDRQpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNApre; //LDRQpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAui; //LDRQui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroW; //LDRQroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRQroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRQPNAroX; //LDRQroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDl:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAl; //LDRDl-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApost; //LDRDpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNApre; //LDRDpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAui; //LDRDui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroW; //LDRDroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRDroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRDPNAroX; //LDRDroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSl:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAl; //LDRSl-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApost; //LDRSpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNApre; //LDRSpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAui; //LDRSui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroW; //LDRSroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSPNAroX; //LDRSroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWl:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAl; //LDRWl-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApost; //LDRWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNApre; //LDRWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAui; //LDRWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroW; //LDRWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRWroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRWPNAroX; //LDRWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXl:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAl; //LDRXl-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApost; //LDRXpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNApre; //LDRXpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAui; //LDRXui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroW; //LDRXroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRXroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRXPNAroX; //LDRXroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApost; //LDRSWpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNApre; //LDRSWpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAui; //LDRSWui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroW; //LDRSWroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRSWroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRSWPNAroX; //LDRSWroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApost; //LDRHHpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNApre; //LDRHHpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAui; //LDRHHui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroW; //LDRHHroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRHHroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRHHPNAroX; //LDRHHroX-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpost:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApost; //LDRBBpost-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBpre:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNApre; //LDRBBpre-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBui:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAui; //LDRBBui-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroW:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAroW; //LDRBBroW-No-Predict-Alias
        else continue;
        break;
    
      case AArch64::LDRBBroX:
        if (hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE))
          NewOpcode = AArch64::LDRBBPNAroX; //LDRBBroX-No-Predict-Alias
        else continue;
        break;

      default:
        // if (!MI.mayStore() && hasHint(MI, PREDICT_NO_ALIAS_ADDRESS_SPACE)){
        //   printf("found MI with hint but no match!\n");
        //   MI.print(llvm::outs());
        //   printf("\n");
        // }
        continue;
    }
    MI.PND = true;
    PNDOffsets.push_back(numLoads);
    //MI.setDesc(TII->get(NewOpcode));
    NumOpcodesReplaced += 1;
  }
}

bool AArch64LoadAliasMetadataInsertion::runOnMachineFunction(MachineFunction &MF) {

  if (skipFunction(MF.getFunction())){
    return false;
  }

  std::vector<unsigned> PNDOffsets = std::vector<unsigned>();
  numLoads = 0;

  TRI = MF.getSubtarget().getRegisterInfo();
  TII = MF.getSubtarget<AArch64Subtarget>().getInstrInfo();
  MRI = &MF.getRegInfo();
  LLVM_DEBUG(dbgs() << "***** AArch64LoadAliasMetadataInsertion *****\n");
  Changed = false;
  for (auto &MBB : MF)
    processMachineBasicBlock(MBB, PNDOffsets);
  if (PNDOffsets.size() > 0){
    //Changed = 1;
    errs() << "FUNC:" << MF.getName() << ":";
    int i;
    for (i=0; i < PNDOffsets.size() - 1; i++) errs() << PNDOffsets[i] << ",";
    errs() << PNDOffsets[i] << "\n";
  }
  return Changed;
}

FunctionPass *llvm::createAArch64LoadAliasMetadataInsertion() {
  return new AArch64LoadAliasMetadataInsertion();
}
