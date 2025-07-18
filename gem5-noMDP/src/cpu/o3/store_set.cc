/*
 * Copyright (c) 2004-2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cpu/o3/store_set.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/StoreSet.hh"
#include "cpu/o3/inst_queue.hh"


namespace gem5
{

namespace o3
{

StoreSet::StoreSet(const BaseO3CPUParams &params, MemDepUnit *_memDep)
    : clearPeriod(params.store_set_clear_period), SSITSize(params.SSITSize), LFSTSize(params.LFSTSize), memDep(_memDep)
{
    DPRINTF(StoreSet, "StoreSet: Creating store set object.\n");
    DPRINTF(StoreSet, "StoreSet: SSIT size: %i, LFST size: %i.\n",
            SSITSize, LFSTSize);

    if (!isPowerOf2(SSITSize)) {
        fatal("Invalid SSIT size!\n");
    }

    SSIT.resize(SSITSize);

    validSSIT.resize(SSITSize);

    for (int i = 0; i < SSITSize; ++i)
        validSSIT[i] = false;

    if (!isPowerOf2(LFSTSize)) {
        fatal("Invalid LFST size!\n");
    }

    LFST.resize(LFSTSize);

    validLFST.resize(LFSTSize);

    depCheckShift = params.LSQDepCheckShift;

    for (int i = 0; i < LFSTSize; ++i) {
        validLFST[i] = false;
        LFST[i] = 0;
    }

    indexMask = SSITSize - 1;

    offsetBits = 2;

    memOpsPred = 0;
}

StoreSet::~StoreSet()
{
}

void
StoreSet::init(const BaseO3CPUParams &params, MemDepUnit *_memDep)
{
    SSITSize = params.SSITSize;
    LFSTSize = params.LFSTSize;
    clearPeriod = params.store_set_clear_period;
    memDep = _memDep;

    DPRINTF(StoreSet, "StoreSet: Creating store set object.\n");
    DPRINTF(StoreSet, "StoreSet: SSIT size: %i, LFST size: %i.\n",
            SSITSize, LFSTSize);

    SSIT.resize(SSITSize);

    validSSIT.resize(SSITSize);

    for (int i = 0; i < SSITSize; ++i)
        validSSIT[i] = false;

    LFST.resize(LFSTSize);

    validLFST.resize(LFSTSize);

    depCheckShift = params.LSQDepCheckShift;

    for (int i = 0; i < LFSTSize; ++i) {
        validLFST[i] = false;
        LFST[i] = 0;
    }

    indexMask = SSITSize - 1;

    offsetBits = 2;

    memOpsPred = 0;

}


void StoreSet::violation(Addr load_PC, InstSeqNum load_seq_num, InstSeqNum store_seq_num, Addr store_PC, std::ptrdiff_t storeQueueDistance, bool predicted, unsigned predictedPathInex, uint64_t predictedHash, BranchHistory branchHistory) 
{
    int load_index = calcIndex(load_PC);
    int store_index = calcIndex(store_PC);

    assert(load_index < SSITSize && store_index < SSITSize);

    bool valid_load_SSID = validSSIT[load_index];
    bool valid_store_SSID = validSSIT[store_index];

    if (!valid_load_SSID && !valid_store_SSID) {
        // Calculate a new SSID here.
        SSID new_set = calcSSID(load_PC);


        validSSIT[load_index] = true;

        SSIT[load_index] = new_set;

        validSSIT[store_index] = true;

        SSIT[store_index] = new_set;

        assert(new_set < LFSTSize);

        intended_index[load_index] = load_PC;
        intended_index[store_index] = store_PC;

        DPRINTF(StoreSet, "StoreSet: Neither load nor store had a valid "
                "storeset, creating a new one: %i for load %#x, store %#x\n",
                new_set, load_PC, store_PC);
    } else if (valid_load_SSID && !valid_store_SSID) {
        SSID load_SSID = SSIT[load_index];

        validSSIT[store_index] = true;

        SSIT[store_index] = load_SSID;

        assert(load_SSID < LFSTSize);

        intended_index[store_index] = store_PC;

        DPRINTF(StoreSet, "StoreSet: Load had a valid store set.  Adding "
                "store to that set: %i for load %#x, store %#x\n",
                load_SSID, load_PC, store_PC);
    } else if (!valid_load_SSID && valid_store_SSID) {
        SSID store_SSID = SSIT[store_index];

        validSSIT[load_index] = true;

        SSIT[load_index] = store_SSID;

        intended_index[load_index] = load_PC;

        DPRINTF(StoreSet, "StoreSet: Store had a valid store set: %i for "
                "load %#x, store %#x\n",
                store_SSID, load_PC, store_PC);
    } else {
        SSID load_SSID = SSIT[load_index];
        SSID store_SSID = SSIT[store_index];

        assert(load_SSID < LFSTSize && store_SSID < LFSTSize);

        // The store set with the lower number wins
        if (store_SSID > load_SSID) {
            SSIT[store_index] = load_SSID;
            intended_index[store_index] = store_PC;

            DPRINTF(StoreSet, "StoreSet: Load had smaller store set: %i; "
                    "for load %#x, store %#x\n",
                    load_SSID, load_PC, store_PC);
        } else {
            SSIT[load_index] = store_SSID;
            intended_index[load_index] = load_PC;

            DPRINTF(StoreSet, "StoreSet: Store had smaller store set: %i; "
                    "for load %#x, store %#x\n",
                    store_SSID, load_PC, store_PC);
        }
    }
}

void StoreSet::commit(Addr load_pc, Addr load_addr, unsigned load_size, Addr store_addr, unsigned store_size, unsigned path_index, uint64_t predictor_hash) {

    bool misprediction;
    Addr load_eff_addr1 = load_addr >> depCheckShift;
    Addr load_eff_addr2 = (load_addr + load_size - 1) >> depCheckShift;
    Addr store_eff_addr1 = store_addr >> depCheckShift;
    Addr store_eff_addr2 = (store_addr + store_size - 1) >> depCheckShift;
    if (store_eff_addr2 >= load_eff_addr1 && store_eff_addr1 <= load_eff_addr2)
        misprediction = false;
    else
        misprediction = true;

    if (misprediction) ++(memDep->stats.PHASTMispredictions);
    else ++(memDep->stats.PHASTCorrectPredictions);
}

void
StoreSet::checkClear()
{
    memOpsPred++;
    if (memOpsPred > clearPeriod) {
        DPRINTF(StoreSet, "Wiping predictor state beacuse %d ld/st executed\n",
                clearPeriod);
        memOpsPred = 0;
        clear();
    }
}

void
StoreSet::insertLoad(Addr load_PC, InstSeqNum load_seq_num)
{
    checkClear();
    // Does nothing.
    return;
}

void
StoreSet::insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid)
{
    int index = calcIndex(store_PC);

    int store_SSID;

    checkClear();
    assert(index < SSITSize);

    if (!validSSIT[index]) {
        // Do nothing if there's no valid entry.
        return;
    } else {
        store_SSID = SSIT[index];

        assert(store_SSID < LFSTSize);

        // Update the last store that was fetched with the current one.
        LFST[store_SSID] = store_seq_num;

        validLFST[store_SSID] = 1;

        ++(memDep->stats).LFSTWrites;

        storeList[store_seq_num] = store_SSID;

        DPRINTF(StoreSet, "Store %#x updated the LFST, SSID: %i\n",
                store_PC, store_SSID);
    }
}

PredictionResult StoreSet::checkInst(Addr PC, InstSeqNum load_seq_num, BranchHistory branchHistory, bool isLoad)
{

    struct PredictionResult prediction = {std::vector<InstSeqNum>(),0,0,0};

    int index = calcIndex(PC);

    int inst_SSID;

    assert(index < SSITSize);

    if (!validSSIT[index]) {
        DPRINTF(StoreSet, "Inst %#x with index %i had no SSID\n",
                PC, index);

        // Return 0 if there's no valid entry.
        return prediction;
    } else {

        inst_SSID = SSIT[index];

        assert(inst_SSID < LFSTSize);

        ++(memDep->stats).LFSTReads;

        if (!validLFST[inst_SSID]) {

            DPRINTF(StoreSet, "Inst %#x with index %i and SSID %i had no "
                    "dependency\n", PC, index, inst_SSID);

            return prediction;
        } else {
            DPRINTF(StoreSet, "Inst %#x with index %i and SSID %i had LFST "
                    "inum of %i\n", PC, index, inst_SSID, LFST[inst_SSID]);

            prediction.seqNums.push_back(LFST[inst_SSID]);
            return prediction;
        }
    }
}

void
StoreSet::issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store)
{
    // This only is updated upon a store being issued.
    if (!is_store) {
        return;
    }

    int index = calcIndex(issued_PC);

    int store_SSID;

    assert(index < SSITSize);

    SeqNumMapIt store_list_it = storeList.find(issued_seq_num);

    if (store_list_it != storeList.end()) {
        storeList.erase(store_list_it);
    }

    // Make sure the SSIT still has a valid entry for the issued store.
    if (!validSSIT[index]) {
        return;
    }

    store_SSID = SSIT[index];

    assert(store_SSID < LFSTSize);

    // If the last fetched store in the store set refers to the store that
    // was just issued, then invalidate the entry.
    if (validLFST[store_SSID] && LFST[store_SSID] == issued_seq_num) {
        DPRINTF(StoreSet, "StoreSet: store invalidated itself in LFST.\n");
        validLFST[store_SSID] = false;
        ++(memDep->stats).LFSTReads;
        ++(memDep->stats).LFSTWrites;
    }
}

void
StoreSet::squash(InstSeqNum squashed_num, ThreadID tid)
{
    DPRINTF(StoreSet, "StoreSet: Squashing until inum %i\n",
            squashed_num);

    int idx;
    SeqNumMapIt store_list_it = storeList.begin();

    //@todo:Fix to only delete from correct thread
    while (!storeList.empty()) {
        idx = (*store_list_it).second;

        if ((*store_list_it).first <= squashed_num) {
            break;
        }

        bool younger = LFST[idx] > squashed_num;

        if (validLFST[idx] && younger) {
            DPRINTF(StoreSet, "Squashed [sn:%lli]\n", LFST[idx]);
            validLFST[idx] = false;

            storeList.erase(store_list_it++);
        } else if (!validLFST[idx] && younger) {
            storeList.erase(store_list_it++);
        }
    }
}

void
StoreSet::clear()
{
    for (int i = 0; i < SSITSize; ++i) {
        validSSIT[i] = false;
    }

    for (int i = 0; i < LFSTSize; ++i) {
        validLFST[i] = false;
    }

    storeList.clear();

    intended_index.clear();

}

void
StoreSet::dump()
{
    cprintf("storeList.size(): %i\n", storeList.size());
    SeqNumMapIt store_list_it = storeList.begin();

    int num = 0;

    while (store_list_it != storeList.end()) {
        cprintf("%i: [sn:%lli] SSID:%i\n",
                num, (*store_list_it).first, (*store_list_it).second);
        num++;
        store_list_it++;
    }
}

} // namespace o3
} // namespace gem5
