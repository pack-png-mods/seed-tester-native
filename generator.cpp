#include "generator.h"

#define WATERFALL_X 14
#define WATERFALL_Y 76
#define WATERFALL_Z 10
#define TREE1_X WATERFALL_X - 5 // Left tree on the image
#define TREE1_Z WATERFALL_Z - 8
#define TREE1_HEIGHT 5
#define TREE2_X WATERFALL_X - 3 // right tree on the image
#define TREE2_Z WATERFALL_Z + 3
#define TREE2_HEIGHT 5
#define TREE3_MIN_X WATERFALL_X + 3 // blob
#define TREE3_MAX_X WATERFALL_X + 5 // blob
#define TREE3_MIN_Z WATERFALL_Z - 9
#define TREE3_MAX_Z WATERFALL_Z - 6
#define THIRD_TREE_ENABLED false

bool generator::ChunkGenerator::checkWaterfalls(random_math::JavaRand random)
{
    for (int i = 0; i < 50; i++) {
        int x = random.nextInt(16);
        int y = random.nextInt(random.nextInt(120) + 8);
        int z = random.nextInt(16);
        if (x == WATERFALL_X && y == WATERFALL_Y && z == WATERFALL_Z)
            return true;
        if (x == WATERFALL_X) {
            if (y <= WATERFALL_Y + 1 && y >= WATERFALL_Y && std::abs(z - WATERFALL_Z) <= 2)
                return false;
        }
    }
    return false;
}

void generator::ChunkGenerator::simulateDecorations(random_math::JavaRand random)
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


bool generator::ChunkGenerator::isValidTreeSpot(int treeX, int treeZ, bool firstTreeFound, bool secondTreeFound)
{
    if (treeZ >= WATERFALL_Z - 1 && treeZ <= WATERFALL_Z + 1 && treeX <= WATERFALL_X - 3 && treeX >= WATERFALL_X - 5)
        return true;
    if (!firstTreeFound) {
        if (treeX >= TREE1_X - 1 && treeX <= TREE1_X + 1 && treeZ >= TREE1_Z - 1 && treeZ <= TREE1_Z + 1)
            return true;
    }
    if (!secondTreeFound) {
        if (treeX >= TREE2_X - 1 && treeX <= TREE2_X + 1 && treeZ >= TREE2_Z - 1 && treeZ <= TREE2_Z + 1)
            return true;
    }
    return false;
}

bool *generator::ChunkGenerator::generateLeafPattern(random_math::JavaRand random)
{
    bool *out = new bool[16];
    for (int i = 0; i < 16; ++i) {
        out[i] = random.nextInt(2) != 0;
    }
    return out;
}

int32_t generator::ChunkGenerator::checkTrees(random_math::JavaRand random, int32_t maxTreeCount)
{
    bool treesFound[3] = {false, false, false};
    int8_t foundTreeCount = 0;
    for (int i = 0; i <= maxTreeCount; ++i) {
        int8_t treeX = random.nextInt(16);
        int8_t treeZ = random.nextInt(16);
        int8_t height = random.nextInt(3) + 4;
        if (!treesFound[0] && treeX == TREE1_X && treeZ == TREE1_Z && height == TREE1_HEIGHT) {
            delete[] generator::ChunkGenerator::generateLeafPattern(random);
            foundTreeCount++;
            treesFound[0] = true;
        } else if (!treesFound[1] && treeX == TREE2_X && treeZ == TREE2_Z && height == TREE2_HEIGHT) {
            bool *leafPattern = generateLeafPattern(random);
            if (!leafPattern[0] && leafPattern[4]) {
                foundTreeCount++;
                treesFound[1] = true;
            } else {
                return -1;
            }
            delete[] leafPattern;
        } else if (THIRD_TREE_ENABLED) {
            if (!treesFound[2] && treeX >= TREE3_MIN_X && treeX <= TREE3_MAX_X && treeZ >= TREE3_MIN_Z &&
                treeZ <= TREE3_MAX_Z) {
                delete[] generateLeafPattern(random);
                foundTreeCount++;
                treesFound[2] = true;
            }
        } else {
            if (isValidTreeSpot(treeX, treeZ, treesFound[0], treesFound[1]))
                return -1;
        }

        if ((THIRD_TREE_ENABLED && foundTreeCount == 3) || (!THIRD_TREE_ENABLED && foundTreeCount == 2))
            return i;
    }
    return -1;
}

bool generator::ChunkGenerator::populate(int64_t chunkSeed)
{
    random_math::JavaRand random(advance_3759.next(chunkSeed), false);

    int32_t maxBaseTreeCount = 12;
    if (random.nextInt(10) == 0)
        return false;

    int32_t usedTrees = ChunkGenerator::checkTrees(random, maxBaseTreeCount);
    if (usedTrees == -1)
        return false;

    int64_t seed = random.getSeed();

    for (int i = usedTrees; i <= maxBaseTreeCount; ++i) {
        ChunkGenerator::simulateDecorations(random);
        if (ChunkGenerator::checkWaterfalls(random))
            return true;

        random.setSeed(seed, false);
        int32_t treeX = random.nextInt(16);
        int32_t treeZ = random.nextInt(16);
        if (ChunkGenerator::isValidTreeSpot(treeX, treeZ, true, true))
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