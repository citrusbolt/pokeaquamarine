#include "global.h"
#include "random.h"

// The number 1103515245 comes from the example implementation
// of rand and srand in the ISO C standard.

EWRAM_DATA static u32 sRandCount = 0;

COMMON_DATA u32 gRngValue = 0;
COMMON_DATA u32 gRng2Value = 0;

u16 Random(void)
{
    gRngValue = ISO_RANDOMIZE1(gRngValue);
    return gRngValue >> 16;
}

void SeedRng(u16 seed)
{
    gRngValue = seed;
}

void SeedRng2(u16 seed)
{
    gRng2Value = seed;
}

u16 Random2(void)
{
    gRng2Value = ISO_RANDOMIZE1(gRng2Value);
    return;
}
