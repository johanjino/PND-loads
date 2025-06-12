#include "store_distance.hh"


namespace gem5
{
    extern std::map<uint64_t, std::map<uint64_t, uint64_t>> loads_map;
    extern std::unordered_map<uint64_t, uint64_t> load_exec_count;

    uint64_t depCheckShift = 4;


    // IF USING UNORDERED_MAP

    // Clean-call for stores
    void record_store(uint64_t addr, uint64_t size) {
        store_counter++;
        for(int i=0; i<size; i++) store_map[ (addr+i) >> depCheckShift ] = store_counter;
    }

    // Clean-call for loads
    void record_load(uint64_t load, uint64_t addr, uint64_t size) {
        uint64_t last = 0;
        bool found = false;
        for(int i=0; i<size; i++){ 
            uint64_t address_to_check = (addr+i) >> depCheckShift;
            if (store_map.find(address_to_check)!=store_map.end()){
                    last = std::max(store_map[address_to_check], last);
                    found = true;
            }
        }
        uint64_t distance;
        if (!found){
            distance = limit+1;   
        }
        else{
            distance = (store_counter-last>limit ? limit : store_counter-last);
        }
        loads_map[load][distance]++;
        load_exec_count[load]++;
    }


    // IF USING LRA

    // Clean-call for stores
    // void record_store(uint64_t addr, uint64_t size) {
    //     store_counter++;
    //     for(int i=0; i<size; i++) store_map.put(((addr+i) >> depCheckShift),store_counter);
    // }

    // // Clean-call for loads
    // void record_load(uint64_t load, uint64_t addr, uint64_t size) {
    //     uint64_t last = 0;
    //     bool found = false;
    //     for(int i=0; i<size; i++){ 
    //         uint64_t address_to_check = (addr+i) >> depCheckShift;
    //         if (store_map.exists(address_to_check)){
    //                 last = std::max(store_map.get(address_to_check), last);
    //                 found = true;
    //         }
    //     }
    //     uint64_t distance;
    //     if (!found){
    //         distance = limit+1;   
    //     }
    //     else{
    //         distance = (store_counter-last>limit ? limit : store_counter-last);
    //     }
    //     loads_map[load][distance]++;
    //     load_exec_count[load]++;
    // }
}