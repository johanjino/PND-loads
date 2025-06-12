#ifndef __GLOBAL_PND_STATS_HH__
#define __GLOBAL_PND_STATS_HH__

#include <unordered_map>
#include <set>
#include <cstdint>

extern std::unordered_map<uint64_t, uint64_t> pnd_violation_count;
extern std::unordered_map<uint64_t, std::set<uint64_t>> pnd_violating_stores;
extern std::unordered_map<uint64_t, uint64_t> pnd_exec_count;

#endif // __GLOBAL_PND_STATS_HH__