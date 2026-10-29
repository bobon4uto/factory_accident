#ifdef    MONO_BUILD
#define RANDOM_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _RANDOM_H_
#define    _RANDOM_H_
// random interface

#include "noutil.h"

void random_seed(u seed);
float random_float_in_range(float from, float to);
bool random_bool();
u32 random_u32();

#ifdef      RANDOM_IMPLEMENTATION
// random implementation
static uint32_t pcg32(void);
static void pcg32_init(uint64_t seed);

static float uint32_to_float(uint32_t uint) {
  return (float)uint / (float)UINT32_MAX;
}
float random_float_in_range(float from, float to) {
  float f0_to_1 = uint32_to_float( pcg32() );
  return from + (to - from)*f0_to_1;
}
bool random_bool() {
  return (bool)(pcg32()%2) ;
}
u32 random_u32() {
  return (int)(pcg32()) ;
}
void random_seed(u seed) {
  pcg32_init((uint64_t)seed);
}





// from https://en.wikipedia.org/wiki/Permuted_congruential_generator
#include <stdint.h>

static uint64_t       state      = 0x00000000d34df00d;
static uint64_t const multiplier = 6364136223846793005u;
static uint64_t const increment  = 1442695040888963407u;	// Or an arbitrary odd constant

static uint32_t rotr32(uint32_t x, unsigned r) {
  return x >> r | x << (-r & 31);
}


static uint32_t pcg32(void) {
  uint64_t x = state;
  unsigned count = (unsigned)(x >> 59);       // 59 = 64 - 5


  state = x * multiplier + increment;
  x ^= x >> 18;                               // 18 = (64 - 27)/2
  return rotr32((uint32_t)(x >> 27), count);  // 27 = 32 - 5
}



static void pcg32_init(uint64_t seed)
{
  state = seed + increment;
  (void)pcg32();
}

#endif   // RANDOM_IMPLEMENTATION
#endif   //_RANDOM_H_

