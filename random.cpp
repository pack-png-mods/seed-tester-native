#include "random.h"

random_math::LCG::LCG(int64_t multiplier, int64_t addend, int64_t modulo, bool maskable)
        : multiplier(multiplier), addend(addend), modulo(modulo), maskable(maskable)
{}

int64_t random_math::LCG::next(int64_t seed)
{
    if (this->maskable)
        return (seed * this->multiplier + this->addend) & (this->modulo - 1);
    return (seed * this->multiplier + this->addend) % this->modulo;
}

random_math::LCG random_math::LCG::combine(int64_t steps)
{
    int64_t mul = 1;
    int64_t add = 0;

    int64_t intermediateMultiplier = this->multiplier;
    int64_t intermediateAddend = this->addend;

    for (int64_t i = 0; i < steps; i >>= 1) {
        if ((i & 1) != 0) {
            mul *= intermediateMultiplier;
            add = intermediateMultiplier * add + intermediateAddend;
        }

        intermediateAddend = (intermediateMultiplier + 1) * intermediateAddend;
        intermediateMultiplier *= intermediateMultiplier;
    }

    if (maskable) {
        mul &= this->modulo - 1;
        add &= this->modulo - 1;
    } else {
        mul %= this->modulo;
        add %= this->modulo;
    }

    return LCG(mul, add, this->modulo);
}

random_math::LCG random_math::JavaRand::lcg = LCG(0x5DEECE66DULL, 0xBULL, 1ULL << 48);

random_math::JavaRand::JavaRand(long seed, bool scramble)
        : seed(0)
{
    this->setSeed(seed, scramble);
}

int64_t random_math::JavaRand::getSeed()
{
    return this->seed;
}

void random_math::JavaRand::setSeed(int64_t seed, bool scramble)
{
    this->seed = seed ^ (scramble ? lcg.multiplier : 0ULL);
    this->seed &= lcg.modulo - 1;
}

int32_t random_math::JavaRand::next(int32_t bits)
{
    this->seed = lcg.next(this->seed);
    return (int32_t) (this->seed >> (48 - bits));
}

int32_t random_math::JavaRand::nextInt(int32_t bound)
{
    if (bound <= 0) {
        throw std::invalid_argument("Bound must be positive");
    }

    if ((bound & -bound) == bound) {
        return (int32_t) ((bound * (int64_t) this->next(31)) >> 31);
    }

    int32_t bits, value;

    do {
        bits = this->next(31);
        value = bits % bound;
    } while (bits - value + (bound - 1) < 0);

    return value;
}