#include <iostream>
#include "generator.h"

#define WATERFALL_Y 76
#define WATERFALL_Z 10
#define TREE2_X -3 // right tree on the image
#define TREE2_Z WATERFALL_Z + 3
#define TREE2_HEIGHT 5
#define TREE3_MIN_X 3 // blob
#define TREE3_MAX_X 5 // blob
#define TREE3_MIN_Z WATERFALL_Z - 9
#define TREE3_MAX_Z WATERFALL_Z - 6
#define THIRD_TREE_ENABLED true

bool generator::ChunkGenerator::checkWaterfalls(random_math::JavaRand& random, int waterfallX)
{
    for (int32_t i = 0; i < 50; i++) {
        int x = random.nextInt(16);
        int y = random.nextInt(random.nextInt(120) + 8);
        int z = random.nextInt(16);
        if (x == waterfallX && y == WATERFALL_Y && z == WATERFALL_Z)
            return true;
    }
    return false;
}

void generator::ChunkGenerator::simulateDecorations(random_math::JavaRand& random)
{
    random.setSeed(advance_774.next(random.getSeed()), false);
    if (random.nextInt(2) == 0) {
        random.setSeed(advance_387.next(random.getSeed()), false);
    }
    if (random.nextInt(4) == 0) {
        random.setSeed(advance_387.next(random.getSeed()), false);
    }
    if (random.nextInt(8) == 0) {
        random.setSeed(advance_387.next(random.getSeed()), false);
    }
    random.setSeed(advance_830.next(random.getSeed()), false);
    if (random.nextInt(32) == 0) {
        random.setSeed(advance_387.next(random.getSeed()), false);
    }
}


bool generator::ChunkGenerator::isValidTreeSpot(int treeX, int treeZ, bool secondTreeFound, int waterfallX)
{
    if (treeZ >= WATERFALL_Z - 1 && treeZ <= WATERFALL_Z + 1 && treeX <= waterfallX - 3 && treeX >= waterfallX - 5)
        return true;
    if (!secondTreeFound) {
        if (treeX >= waterfallX + TREE2_X - 1 && treeX <= waterfallX + TREE2_X + 1 && treeZ >= TREE2_Z - 1 && treeZ <= TREE2_Z + 1)
            return true;
    }
    return false;
}

bool *generator::ChunkGenerator::generateLeafPattern(random_math::JavaRand& random)
{
    bool *out = new bool[16];
    for (int32_t i = 0; i < 16; i++) {
        out[i] = random.nextInt(2) != 0;
    }
    return out;
}

int32_t generator::ChunkGenerator::checkTrees(random_math::JavaRand& random, int32_t maxTreeCount, int waterfallX)
{
    bool treesFound[3] = {false, false, false};
    int8_t foundTreeCount = 0;
    for (int i = 0; i <= maxTreeCount; ++i) {
        int32_t treeX = random.nextInt(16);
        int32_t treeZ = random.nextInt(16);
        int32_t height = random.nextInt(3) + 4;
        if (!treesFound[1] && treeX == waterfallX + TREE2_X && treeZ == TREE2_Z && height == TREE2_HEIGHT) {
            bool *leafPattern = generateLeafPattern(random);
            if (!leafPattern[0] && leafPattern[4]) {
                foundTreeCount++;
                treesFound[1] = true;
            } else {
                return -1;
            }
            delete[] leafPattern;
        } else if (THIRD_TREE_ENABLED && !treesFound[2] && treeX >= waterfallX + TREE3_MIN_X && treeX <= waterfallX + TREE3_MAX_X && treeZ >= TREE3_MIN_Z &&
                    treeZ <= TREE3_MAX_Z) {
            delete[] generateLeafPattern(random);
            foundTreeCount++;
            treesFound[2] = true;
        } else {
            if (isValidTreeSpot(treeX, treeZ, treesFound[1], waterfallX))
                return -1;
        }

        if (foundTreeCount == 2)
            return i;
    }
    return -1;
}

bool generator::ChunkGenerator::populate(int64_t chunkSeed, int waterfallX, int *uTrees)
{
    random_math::JavaRand random(advance_3759.next(chunkSeed), false);

    int32_t maxBaseTreeCount = 12;
    if (random.nextInt(10) == 0)
        return false;

    int32_t usedTrees = ChunkGenerator::checkTrees(random, maxBaseTreeCount, waterfallX);
    if (usedTrees == -1)
        return false;


    int64_t seed = random.getSeed();
    for (int i = usedTrees; i <= maxBaseTreeCount; ++i) {

        *uTrees = i;
        ChunkGenerator::simulateDecorations(random);
        if (ChunkGenerator::checkWaterfalls(random, waterfallX))
            return true;

        random.setSeed(seed, false);
        int32_t treeX = random.nextInt(16);
        int32_t treeZ = random.nextInt(16);
        if (ChunkGenerator::isValidTreeSpot(treeX, treeZ, true, waterfallX))
            return false;
        random.nextInt(3);
        seed = random.getSeed();
    }
    return false;
}

random_math::LCG generator::ChunkGenerator::advance_387 = random_math::JavaRand::lcg.combine(387);
random_math::LCG generator::ChunkGenerator::advance_774 = random_math::JavaRand::lcg.combine(774);
random_math::LCG generator::ChunkGenerator::advance_830 = random_math::JavaRand::lcg.combine(830);
random_math::LCG generator::ChunkGenerator::advance_3759 = random_math::JavaRand::lcg.combine(3759);