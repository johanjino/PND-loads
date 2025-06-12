#include <stdint.h>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <iostream>
#include <list>


struct load_obj{
    uint64_t exec_count = 0;
    std::unordered_map<uint64_t, uint64_t> alias_store_map = {};
};

bool check_in(uint64_t inst);
void record_profile_store(uint64_t store, uint64_t addr, uint64_t size);
void record_profile_load(uint64_t load, uint64_t addr, uint64_t size);
