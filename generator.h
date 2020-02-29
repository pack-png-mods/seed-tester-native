#ifndef SEED_TESTER_GENERATOR_H
#define SEED_TESTER_GENERATOR_H

#include "random.h"
#include <cstdint>

namespace generator
{

    class ChunkGenerator
    {
    private:
        static bool checkWaterfalls(random_math::JavaRand& random);
        static void simulateDecorations(random_math::JavaRand& random);
        static bool isValidTreeSpot(int treeX, int treeZ, bool firstTreeFound, bool secondTreeFound);
        static bool *generateLeafPattern(random_math::JavaRand& random);
        static int32_t checkTrees(random_math::JavaRand& random, int32_t maxTreeCount);
    public:
        static random_math::LCG advance_387;
        static random_math::LCG advance_774;
        static random_math::LCG advance_830;
        static random_math::LCG advance_3759;
        static bool populate(int64_t chunkSeed);

    };
}


#endif //SEED_TESTER_GENERATOR_H
