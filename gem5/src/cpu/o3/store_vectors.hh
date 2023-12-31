/*
 * Copyright (c) 2004-2005 The Regents of The University of Michigan
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

#ifndef __CPU_O3_STORE_SET_HH__
#define __CPU_O3_STORE_SET_HH__

#include <list>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base/statistics.hh"
#include "base/types.hh"
#include "cpu/inst_seq.hh"
#include "cpu/o3/dyn_inst_ptr.hh"
#include "cpu/o3/limits.hh"
#include "debug/MemDepUnit.hh"
#include "mem/packet.hh"
#include "mem/port.hh"


namespace gem5
{

namespace o3
{

class MemDepUnit;

struct ltseqnum
{
    bool
    operator()(const InstSeqNum &lhs, const InstSeqNum &rhs) const
    {
        return lhs > rhs;
    }
};

/**
 * Implements paper
 */
class StoreVector
{
  public:
    /* NOTE:
    *  In hardware, store vectors uses a Load Schedule Matrix (LSM) to relate
    *  relative SQ offsets in SVT entries to absolute SQ indecies. In Gem5 though, we
    *  can have sequence numbers directly, so we may as well just track those.
    *  We store the most recently violating store PC in the SVT entry, then
    *  map store PCs to their most recently issued seq num.
    *  This is less realistic but won't change any behaviour in the algorithm.
    */
    //8bit partial tag (LSBs of load PC) + PC of last offending store
    typedef std::pair<uint8_t, Addr> SVTEntry;

    /** Default constructor.  init() must be called prior to use. */
    StoreVector() { };

    /** Creates store vector predictor with given table sizes. */
    StoreVector(uint64_t clear_period, int SSIT_size, int LFST_size, MemDepUnit *mem_dep);

    /** Default destructor. */
    ~StoreVector();

    /** Initializes the store vector predictor with the given table sizes. */
    void init(uint64_t clear_period, int SSIT_size, int LFST_size, MemDepUnit *mem_dep);

    /** Records a memory ordering violation between the younger load
     * and the older store. */
    void violation(Addr store_PC, Addr load_PC);

    /** Clears the store vector predictor every so often so that all the
     * entries aren't used and stores are constantly predicted as
     * conflicting.
     */
    void checkClear();

    /** Inserts a load into the store vector predictor.  This does nothing but
     * is included in case other predictors require a similar function.
     */
    void insertLoad(Addr load_PC, InstSeqNum load_seq_num);

    /** Inserts a store into the store vector predictor.  Updates the
     * LFST if the store has a valid SSID. */
    void insertStore(Addr store_PC, InstSeqNum store_seq_num, ThreadID tid);

    /** Checks if the instruction with the given PC is dependent upon
     * any store.  @return Returns the sequence number of the store
     * instruction this PC is dependent upon.  Returns 0 if none.
     */
    InstSeqNum checkInst(Addr PC);

    /** Records this PC/sequence number as issued. */
    void issued(Addr issued_PC, InstSeqNum issued_seq_num, bool is_store);

    /** Squashes for a specific thread until the given sequence number. */
    void squash(InstSeqNum squashed_num, ThreadID tid);

    /** Resets all tables. */
    void clear();

    /** Debug function to dump the contents of the store list. */
    void dump();

  private:
    /** Calculates the index into the SSIT based on the PC. */
    inline int calcIndex(Addr PC)
    { return (PC >> offsetBits) & indexMask; }

    /** Takes the least significant 8 bits of a load PC. */
    inline uint8_t getTag(Addr PC)
    { return (PC & 0xff); }

    /** The Store Vector Table. */
    std::vector<SVTEntry> SVT;

    /** Map store PCs to the last seq num they issued with */
    std::map<Addr, InstSeqNum> SeqNumMap;

    /** Bit vector to tell if the SVT has a valid entry. */
    std::vector<bool> validSVT;

    /** Number of loads/stores to process before wiping predictor so all
     * entries don't get saturated
     */
    uint64_t clearPeriod;

    /** Store Vector ID Table size, in entries. */
    int SVTSize;

    /** Mask to obtain the index. */
    int indexMask;

    // HACK: Hardcoded for now.
    int offsetBits;

    /** Number of memory operations predicted since last clear of predictor */
    int memOpsPred;

    MemDepUnit *memDep;
};

} // namespace o3
} // namespace gem5

#endif // __CPU_O3_STORE_SET_HH__
