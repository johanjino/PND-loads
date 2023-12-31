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

#include "cpu/o3/store_vector.hh"

#include "base/intmath.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/StoreSet.hh"
#include "base/compiler.hh"
#include "base/debug.hh"
#include "cpu/o3/dyn_inst.hh"
#include "cpu/o3/inst_queue.hh"
#include "cpu/o3/limits.hh"
#include "debug/Counter.hh"
#include "debug/DefAlias.hh"
#include "debug/DefNotAlias.hh"
#include "debug/flagcheck.hh"
#include "debug/LSQUNITisnotLoadviolation.hh"
#include "debug/MemDepUnit.hh"
#include "debug/Speculate.hh"
#include "params/BaseO3CPU.hh"


namespace gem5
{

namespace o3
{

StoreVector::StoreVector(uint64_t clear_period, int _SVT_size, int _LFST_size, MemDepUnit *mem_dep)
    : clearPeriod(clear_period), SVTSize(_SVT_size)
{

    if (!isPowerOf2(SVTSize)) {
        fatal("Invalid SVT size!\n");
    }

    SVT.resize(SVTSize);

    validSVT.resize(SVTSize);

    for (int i = 0; i < SVTSize; ++i)
        validSVT[i] = false;

    indexMask = SVTSize - 1;

    offsetBits = 2;

    memOpsPred = 0;
}

StoreVector::~StoreVector()
{
}

void
StoreVector::init(uint64_t clear_period, int _SVT_size, int _LFST_Size, MemDepUnit *mem_dep)
{
    SVTSize = _SVT_size;
    clearPeriod = clear_period;
    memDep = mem_dep;

    SVT.resize(SVTSize);

    validSVT.resize(SVTSize);

    for (int i = 0; i < SVTSize; ++i)
        validSVT[i] = false;

    indexMask = SVTSize - 1;

    offsetBits = 2;

    memOpsPred = 0;

    std::string stats_group_name = csprintf("StoreVector__%i", memDep->id);
    memDep->cp->addStatGroup(stats_group_name.c_str(), &(memDep->stats));
}

void
StoreSet::violation(Addr store_PC, Addr load_PC)
{
    int index = calcIndex(load_PC);
    assert(index < SVTSize);
    bool valid_SVT = validSVT[index];
    uint8_t tag = getTag(load_PC);

    //TODO: should we replace a tag on collision or preserve the old one?
    //for now preserving whats already there, so may need a lower clear period
    //replacing could be better as it means the store that violates more frequntly
    //will be caught more often, but in the 50/50 case it creates ping-ponging which
    //is even worse than just one store repeatedly violating
    if (valid_SVT && tag == SVT[index].first){
        SVT[index].second = store_PC;
        SeqNumMap[store_PC] = 0;
    }
    else if (!valid_SVT){
        SVT[index] = std::pair<uint8_t, Addr>(tag, store_PC);
        SeqNumMap[store_PC] = 0;
        validSVT[index] = true;
    }

}

void
StoreSet::checkClear()
{
    memOpsPred++;
    if (memOpsPred > clearPeriod) {
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
    checkClear();

    SeqNumMap[store_PC] = issued_seq_num;
}

InstSeqNum
StoreSet::checkInst(Addr PC)
{
    int index = calcIndex(PC);

    assert(index < SVTSize);

    if (!validSVT[index])
        return 0;

    SVTEntry entry = SVT[index];
    if (entry.first != getTag(PC))
        return 0;

    return SeqNumMap[entry.second];
}

void
StoreSet::issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store)
{
    return; //does nothing
}

void
StoreSet::squash(InstSeqNum squashed_num, ThreadID tid)
{
    return; //does nothing
}

void
StoreSet::clear()
{
    for (int i = 0; i < SVTSize; ++i) {
        validSVT[i] = false;
    }

    SVT.clear();

    SeqNumMap.clear();
}

void
StoreSet::dump()
{
    unsigned num = 0;
    for (auto entry: SVT){
        cprintf("Entry %d:\n", num);
        cprintf("Tag %d:\n", entry.first);
        cprintf("Last Violating Store Addr %d:\n", entry.second);
        cprintf("Current Dependent Store Seq Num %d:\n", SeqNumMap[entry.second]);
        cprintf("\n");
        num++;
    }
}

} // namespace o3
} // namespace gem5
