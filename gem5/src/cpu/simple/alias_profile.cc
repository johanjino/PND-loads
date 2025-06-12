#include "alias_profile.hh"
#include <fstream>


namespace gem5
{
    static std::unordered_map<uint64_t, uint64_t> addr_map;
    extern std::map<uint64_t, load_obj> load_map;

    uint64_t AliasProfileDepCheckShift = 4;


    bool insts_to_check_initialized = false;
    std::unordered_set<uint64_t> insts_to_check;

    void readAddrFile() {
        const char* filePath = std::getenv("ADDR_FILE");
        if (!filePath) {
            std::cerr << "Error: ADDR_FILE environment variable not set.\n";
            return;
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open file at path: " << filePath << "\n";
            return;
        }

        std::cout << "Using file: " << filePath << "\n";

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()){
                uint64_t addr = std::stoull(line);
                insts_to_check.insert(addr);
            }
        }

        file.close();
}

bool check_in(uint64_t inst){
    if (!insts_to_check_initialized){
        readAddrFile();
        insts_to_check_initialized=true;
    }
    return insts_to_check.find(inst)!=insts_to_check.end();
}

    void record_profile_store(uint64_t store, uint64_t addr, uint64_t size) {
        for(int i=0; i<size; i++) addr_map[ (addr+i) >> AliasProfileDepCheckShift ] = store;
    }

    void record_profile_load(uint64_t load, uint64_t addr, uint64_t size) {        
        load_map[load].exec_count++;
        std::unordered_set<uint64_t> seen_stores;
        for(int i=0; i<size; i++){ 
            if (addr_map.find( (addr+i) >> AliasProfileDepCheckShift )!=addr_map.end() && seen_stores.find(addr_map[ (addr+i) >> AliasProfileDepCheckShift ])==seen_stores.end()){
                uint64_t store = addr_map[ (addr+i) >> AliasProfileDepCheckShift ];
                load_map[load].alias_store_map[store]++;
                seen_stores.insert(store);
            }
        }
    }
}