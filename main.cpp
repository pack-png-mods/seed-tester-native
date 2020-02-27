#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <string>
#include <chrono>
#include <mutex>

#include "generator.h"

void processor(std::ifstream& input, std::string outputFile, bool& done, uint64_t& processed, std::mutex& mtx)
{
    std::ofstream output;
    output.open(outputFile);

    while (input.good()) {
        mtx.lock();
        std::string line;
        getline(input, line);
        mtx.unlock();
        int64_t seed = std::stoull(line);
        if (generator::ChunkGenerator::populate(seed)) {
            processed++;
            output << line << "\n";
        }
    }
}

int main()
{
    std::ifstream input;
    input.open("seeds.txt");

    unsigned int threadCount = std::thread::hardware_concurrency() - 1;
    std::thread threads[threadCount];
    bool done[threadCount];
    uint64_t processed[threadCount];

    std::mutex mtx;

    for (int i = 0; i < threadCount; ++i) {
        std::string fileName = "output" + std::to_string(i) + ".txt";
        done[i] = false;
        processed[i] = 0;
        threads[i] = std::thread(processor, std::ref(input), fileName, std::ref(done[i]), std::ref(processed[i]), std::ref(mtx));

    }

    using namespace std::chrono_literals;
    using namespace std::chrono;
    milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    while (!std::all_of(done, done + threadCount, [](bool d) { return d; })) {
        uint64_t processedCount = 0;
        for (int i = 0; i < threadCount; i++) {
            processedCount += processed[i];
        }
        milliseconds current = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        long diff = (current - start).count();

        std::cout << "Processed " << processedCount << " seeds, uptime: " << diff << "s, speed: " << processedCount / diff << std::endl;
        std::this_thread::sleep_for(1s);

    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

