#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <string>
#include <chrono>

#include "generator.h"

void processor(const std::string inputFile, const std::string outputFile, bool& done, uint64_t& processed, uint64_t& found, int waterfallX)
{
    std::ofstream output;
    output.open(outputFile);

    std::ifstream input;
    input.open(inputFile);


    while (input.good()) {
        std::string line;
        std::getline(input, line);
        int64_t seed = 0;
        try {
            seed = std::stoull(line);
        } catch (std::invalid_argument& ex) {
            std::cout << line << std::endl;
        }
        int usedTrees = 0;
        if (generator::ChunkGenerator::populate(seed, &usedTrees, waterfallX)) {
            output << line << "; " << usedTrees << "\n";
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
    unsigned int threadCount = 1;//std::thread::hardware_concurrency();
    std::thread threads[threadCount];
    bool done[threadCount];
    uint64_t processed[threadCount];
    uint64_t found[threadCount];

    int first = 8;

    for (int i = 0; i < threadCount; ++i) {
        std::string fileName = "out" + std::to_string(i + first) + ".txt";
        std::string inputName = "x" + std::to_string(i + first) + ".txt";
        done[i] = false;
        processed[i] = 0;
        found[i] = 0;
        threads[i] = std::thread(processor, inputName, fileName, std::ref(done[i]), std::ref(processed[i]), std::ref(found[i]), i + first);
    }

    using namespace std::chrono_literals;
    using namespace std::chrono;
    milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

    while (!std::all_of(done, done + threadCount, [](bool d) { return d; })) {

        int doneCount = 0;
        for (int i = 0; i < threadCount; i++) {
            if (done[i])
                doneCount++;
        }

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
                << (double)foundCount / processedCount * 100 << "%, threads finished: " << doneCount << "/" << threadCount << std::endl;
        std::this_thread::sleep_for(0.5s);

    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
