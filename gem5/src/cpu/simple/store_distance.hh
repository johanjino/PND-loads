#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <map>
#include <iostream>
#include <list>


struct AddrObj {
    uint64_t start;
    uint64_t end; // start + size - 1
    uint64_t counter; // Store counter
};


template<typename key_t, typename value_t>
class lra {
public:
	typedef typename std::pair<key_t, value_t> key_value_pair_t;
	typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

	lra(size_t max_size) :
		_max_size(max_size) {
	}
	
	void put(const key_t& key, const value_t& value) {
		// Add new element
		_cache_items_list.push_front(key_value_pair_t(key, value));

		// Remove old value
		auto it = _cache_items_map.find(key);
		if (it != _cache_items_map.end()) {
			_cache_items_list.erase(it->second);
			_cache_items_map.erase(it);
		}

		// Point to new value
		_cache_items_map[key] = _cache_items_list.begin();
		
		// Pop is exceeds size limit
		if (_cache_items_map.size() > _max_size) {
			auto last = _cache_items_list.end();
			last--;
			_cache_items_map.erase(last->first);
			_cache_items_list.pop_back();
		}
	}
	
	const value_t& get(const key_t& key) {
		auto it = _cache_items_map.find(key);
		if (it == _cache_items_map.end()) {
			throw std::range_error("There is no such key in cache");
		} else {
			return it->second->second;
		}
	}
	
	bool exists(const key_t& key) const {
		return _cache_items_map.find(key) != _cache_items_map.end();
	}

	// Kinda inefficient to iterate through the entire list, but limit=1000 should be fine
	// uint64_t check_overlap(uint64_t addr, uint64_t size) {
    // 	uint64_t load_start = addr;
    //     uint64_t load_end = addr + size - 1;

    //     for (const auto& entry : _cache_items_list) {
    //         const AddrObj& stored = entry.second;
    //         if (stored.start <= load_end && stored.end >= load_start) {
    //             return stored.counter; // Overlap detected
    //         }
    //     }
    //     return 0;
    // }


	
	
private:
	std::list<key_value_pair_t> _cache_items_list;
	std::unordered_map<key_t, list_iterator_t> _cache_items_map;
	size_t _max_size;
};

const uint limit = 1000;

static uint64_t store_counter = 0;
// static lra<uint64_t, uint64_t> store_map(100000);
static std::unordered_map<uint64_t, uint64_t> store_map;

// Clean-call for stores
void record_store(uint64_t addr, uint64_t size);
// Clean-call for loads
void record_load(uint64_t load, uint64_t addr, uint64_t size);
