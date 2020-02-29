#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <string>
#include <chrono>

#include "generator.h"

void processor(std::string& inputFile, std::string& outputFile, bool& done, uint64_t& processed, uint64_t& found)
{
    std::ofstream output;
    output.open(outputFile);

    std::ifstream input;
    input.open(inputFile);

    while (input.good()) {
        std::string line;
        std::getline(input, line);
        int64_t seed = std::stoull(line);
        if (generator::ChunkGenerator::populate(seed)) {
            output << line << "\n";
            found++;
        }
        processed++;
    }
    done = true;
    output.close();
    input.close();
}

int main()
{

    unsigned int threadCount = std::thread::hardware_concurrency();
    std::thread threads[threadCount];
    bool done[threadCount];
    uint64_t processed[threadCount];
    uint64_t found[threadCount];

    for (int i = 0; i < threadCount; ++i) {
        std::string fileName = "output" + std::to_string(i) + ".txt";
        std::string inputName = "seeds" + std::to_string(i) + ".txt";
        done[i] = false;
        processed[i] = 0;
        found[i] = 0;
        std::ifstream input;
        threads[i] = std::thread(processor, std::ref(inputName), std::ref(fileName), std::ref(done[i]), std::ref(processed[i]), std::ref(found[i]));
    }

    using namespace std::chrono_literals;
    using namespace std::chrono;
    milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    while (!std::all_of(done, done + threadCount, [](bool d) { return d; })) {
        uint64_t processedCount = 0;
        uint64_t foundCount = 0;
        for (int i = 0; i < threadCount; i++) {
            processedCount += processed[i];
            foundCount += found[i];
        }
        milliseconds current = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        double diff = (double)(current - start).count() / 1000;

        if (diff != 0)
            std::cout << "Processed " << processedCount << " seeds, found " << foundCount << " correct ones, uptime: "
                << diff << "s, speed: " << (processedCount / diff) / 1000000 << "m seed/sec, percentage of correct seeds: "
                << (double)foundCount / processedCount * 100 << "%" << std::endl;
        std::this_thread::sleep_for(0.5s);

    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

