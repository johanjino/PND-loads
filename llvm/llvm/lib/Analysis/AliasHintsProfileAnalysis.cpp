#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasHintsProfileAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace llvm;

inline void AliasHintsProfileAnalysisPass::attachMetadata(LoadInst &I, const std::unordered_map<StoreInst *, bool> &Map) {
    LLVMContext &Ctx = I.getContext();
    std::vector<Metadata *> MetadataEntries;

    for (const auto &Entry : Map) {
        Instruction *Instr = Entry.first;
        bool Value = Entry.second;

        // Only attach instructions with `true` value
        if (Value) {
            Metadata *InstrMeta = ValueAsMetadata::get(Instr);
            MetadataEntries.push_back(InstrMeta);
        }
    }

    // Create a metadata node to store the instructions
    if (!MetadataEntries.empty()) {
        MDNode *InstrMetadata = MDTuple::get(Ctx, MetadataEntries);

        // Attach the metadata to the instruction
        I.setMetadata("AliasedStores", InstrMetadata);
    }
}

PreservedAnalyses AliasHintsProfileAnalysisPass::run(LoopNest &LN, LoopAnalysisManager &AM, LoopStandardAnalysisResults &AR, LPMUpdater &U){

    std::string PNDProfileFilePath = "/rds/general/user/jj21/home/fyp/pnd-loads/sha-profile-filtered.txt";

    std::ifstream file(PNDProfileFilePath);
    if (!file.is_open()) {
        errs() << "Error opening file: " << PNDProfileFilePath << "\n";
        return PreservedAnalyses::all();
    }
    errs() << "Using profile from: " << PNDProfileFilePath << "\n";

    // Step 1: populate Key Map
    std::unordered_map<unsigned, std::unordered_map<unsigned, StoreInst*>> KeyToStoreMap;
    std::unordered_map<unsigned, std::unordered_map<unsigned, LoadInst*>> KeyToLoadMap;
    std::unordered_map<StoreInst*, std::string> StoreToFileMap;
    std::unordered_map<LoadInst*, std::string> LoadToFileMap;
    
    for (auto B: LN.getOutermostLoop().getBlocks()){
        for (Instruction *I = B->getFirstNonPHI(), *Term = B->getTerminator();
                I != Term; I = I->getNextNonDebugInstruction()) {
        //for (BasicBlock &BB : F) {
            //for (Instruction &I : BB) {
                if (const DebugLoc &DL = I->getDebugLoc()) {
        
                    unsigned line = DL.getLine();
                    unsigned column = DL.getCol();
                    
                    std::string FileLoc;
                    if (DILocation *Loc = DL.get()) {
                        // Get the scope (DIScope) of the location
                        DIScope *Scope = Loc->getScope();

                        // Retrieve the file name and directory
                        if (DIFile *File = Scope->getFile()) {
                            FileLoc = File->getDirectory().str() + File->getFilename().str();
                        } else {
                            continue;
                        }
                    }
                    if (isa<LoadInst>(I)) {
                        KeyToLoadMap[line][column] = dyn_cast<LoadInst>(I);
                        LoadToFileMap[dyn_cast<LoadInst>(I)] = FileLoc;
                        //errs() << "Found Load in Loop Nest\n" ;
                    }
                    else if (isa<StoreInst>(I)) {
                        KeyToStoreMap[line][column] = dyn_cast<StoreInst>(I);
                        StoreToFileMap[dyn_cast<StoreInst>(I)] = FileLoc;
                        //errs() << "Found Store in Loop Nest\n";
                    }
                }
            }
        
    }

    // Step 2: poplulate Alias Map
    std::unordered_map<std::string, std::unordered_map<StoreInst*, bool>> AddressToStoreMap; 
    AliasMapType LoadStoreAliasMap;

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string instructionType, filename, line, column, address;
        ss >> instructionType >> filename >> line >> column >> address;

        // Skip invalid lines
        if (filename == "Unknown" || filename.empty() || instructionType.empty() || line.empty() || column.empty() || address.empty())
            continue;
        
        unsigned Line = std::stoi(line, nullptr, 0);
        unsigned Col = std::stoi(column, nullptr, 0);

        if (instructionType == "Store"){
            StoreInst *Store = KeyToStoreMap[Line][Col];
            if (Store == nullptr){
                // This happens here cause profile can have load/store from other files

                // errs() << "Error: Store at "<< line << " "<< column <<" was not found in first instruction pass" << "\n";
                // return false;

                continue;
            }
            if (StoreToFileMap[Store]!=filename){
                // This file and column profile info belongs to another file
                continue;
            }
            AddressToStoreMap[address][Store] = true;
            //errs() << "Updated a Store" << "\n";

        }
        else if(instructionType == "Load"){
            LoadInst *Load = KeyToLoadMap[Line][Col];
            if (Load == nullptr){
                // This happens here cause profile can have load/store from other files

                // errs() << "Error: Store at "<< line << " "<< column <<" was not found in first instruction pass" << "\n";
                // return false;

                continue;
            }
            if (LoadToFileMap[Load]!=filename){
                // This file and column profile info belongs to another file
                continue;
            }
            for(const auto &Entry : AddressToStoreMap[address]){
                StoreInst *AliasedStore = Entry.first;
                LoadStoreAliasMap[Load][AliasedStore] = Entry.second;
            }
            //errs() << "Updated a Load" << "\n";
        }
        else{
            errs() << "Error: Invalid instruction type from profile file "<< PNDProfileFilePath << "\n";
            errs() << "Not using profile information...\n";
            return PreservedAnalyses::all();
        }   
    }
    file.close();

    
    // Step 3: Assign metadata to loads
    // Ideally merge this step with step 2
    for (const auto &Entry : LoadStoreAliasMap){
        attachMetadata(*Entry.first, Entry.second);
    }


    return PreservedAnalyses::all();
}


