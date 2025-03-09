#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>
#include <univalue.h>

// Utility function for timing measurements
template<typename F>
auto measure_time(F&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

void benchmark_object_insertions(int num_keys, bool sequential = true) {
    UniValue obj(UniValue::VOBJ);
    
    std::vector<std::string> keys;
    keys.reserve(num_keys);
    
    // Pre-generate keys
    for (int i = 0; i < num_keys; i++) {
        keys.push_back("key" + std::to_string(i));
    }
    
    if (!sequential) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(keys.begin(), keys.end(), gen);
    }
    
    std::cout << "\nBenchmarking " << num_keys << " key insertions "
              << (sequential ? "(sequential)" : "(random)") << ":\n";
    
    // Measure insertion time
    auto duration = measure_time([&]() {
        for (const auto& key : keys) {
            obj.pushKV(key, 42);
        }
    });
    
    // Measure lookup time
    auto duration_lookup = measure_time([&]() {
        for (const auto& key : keys) {
            const UniValue& val = obj[key];
            if (val.isNull()) {
                std::cerr << "Error: key not found: " << key << "\n";
            }
        }
    });
    
    // Measure serialization time
    auto duration_serialize = measure_time([&]() {
        std::string serialized = obj.write();
    });
    
    std::cout << std::fixed << std::setprecision(2)
              << "  Insertion time:     " << duration.count() << "ms ("
              << (duration.count() * 1000.0 / num_keys) << "µs per key)\n"
              << "  Lookup time:        " << duration_lookup.count() << "ms ("
              << (duration_lookup.count() * 1000.0 / num_keys) << "µs per key)\n"
              << "  Serialization time: " << duration_serialize.count() << "ms\n";
}

int main(int argc, char* argv[]) {
    const int DEFAULT_KEYS = 50000;
    int num_keys = (argc > 1) ? std::stoi(argv[1]) : DEFAULT_KEYS;
    
    benchmark_object_insertions(num_keys, true);
    benchmark_object_insertions(num_keys, false);
    
    return 0;
}