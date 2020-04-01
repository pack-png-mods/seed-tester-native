#ifndef SEED_TESTER_GENERATOR_H
#define SEED_TESTER_GENERATOR_H

#include "random.h"
#include <cstdint>

namespace generator
{

    class ChunkGenerator
    {
    private:
        static bool isValidTreeSpot(int treeX, int treeZ, bool firstTreeFound, bool secondTreeFound, int waterfallX);
        static bool *generateLeafPattern(random_math::JavaRand& random);
        static int32_t checkTrees(random_math::JavaRand& random, int32_t maxTreeCount, int waterfallX);
    public:
        static random_math::LCG advance_3759;
        static bool populate(int64_t chunkSeed, int *uTrees, int waterfallX);

    };
}


#endif //SEED_TESTER_GENERATOR_H
