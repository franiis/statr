/*
 * statr
 * Copyright (C) 2018 Piotr Krzeszewski
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "rng.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h> /* for getpid in TimeToSeed*/
#include <time.h> /* for time() in TimeToSeed */

#define MT_STATE_SIZE 625

static void RNG_Init(Int32 seed);
static void Randomize();
unsigned int TimeToSeed(void);

static double MT_genrand(void);

// statePointer[0] - seed, statePointer[1-624] MT state
Int32 *statePointer;

Int32* get_RNG_state()
{
    return statePointer;
}

void set_seed(Int32 seed)
{
    RNG_Init(seed);
}

Int32 getFirstIntFromState()
{
    return get_RNG_state()[0];
}

// Method is called to check if seed was set. 
// If seed isn't set then it have to be set to use RNG
void validate_RNG(void) 
{
    if (statePointer != NULL) {
        return;
    }
    Randomize();
}

/*
 *  R : A Computer Language for Statistical Data Analysis
 *  Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
 *  Copyright (C) 1997--2016  The R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  https://www.R-project.org/Licenses/
 */

#define i2_32m1 2.328306437080797e-10/* = 1/(2^32 - 1) */

static double fixup(double x)
{
    /* ensure 0 and 1 are never returned */
    if(x <= 0.0) return 0.5*i2_32m1;
    if((1.0 - x) <= 0.0) return 1.0 - 0.5*i2_32m1;
    return x;
}

double unif_rand(void)
{
    validate_RNG();
	return fixup(MT_genrand());
}

static void FixupSeeds(int initial)
{
    int j, notallzero = 0;

	if(initial) {
        statePointer[0] = 624;
    }
	 /* No action unless user has corrupted .Random.seed */
	if( statePointer[0] <= 0) {
        statePointer[0] = 624;
    }
	/* check for all zeroes */
	for (j = 1; j <= 624; j++) {
	    if (statePointer[j] != 0) {
            notallzero = 1;
            break;
        }
	}
	if(!notallzero) { 
        Randomize();
    }
}

static void RNG_Init(Int32 seed)
{
    if (statePointer == NULL) {
        free(statePointer);
    }
    statePointer = (Int32*)malloc(MT_STATE_SIZE * sizeof(Int32));

    int j;
    /* Initial scrambling */
    for(j = 0; j < 50; j++) {
	    seed = (69069 * seed + 1);
    }

	/* i_seed[0] is mti, *but* this is needed for historical consistency */
	for(j = 0; j < 625; j++) {
	    seed = (69069 * seed + 1);
	    statePointer[j] = seed;
	}
	FixupSeeds(1);
}

static void Randomize()
{
    RNG_Init(TimeToSeed());
}

/* ===================  Mersenne Twister ========================== */
/* From http://www.math.keio.ac.jp/~matumoto/emt.html */
/*
   Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura.
   When you use this, send an email to: matumoto@math.keio.ac.jp
   with an appropriate reference to your work.

   REFERENCE
   M. Matsumoto and T. Nishimura,
   "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform
   Pseudo-Random Number Generator",
   ACM Transactions on Modeling and Computer Simulation,
   Vol. 8, No. 1, January 1998, pp 3--30.
*/

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* Initializing the array with a seed */
static void MT_sgenrand(Int32 seed)
{
    Int32 *mt = get_RNG_state() + 1;
    int i;

    for (i = 0; i < N; i++) {
        mt[i] = seed & 0xffff0000;
        seed = 69069 * seed + 1;
        mt[i] |= (seed & 0xffff0000) >> 16;
        seed = 69069 * seed + 1;
    }
    mti = N;
}

/* Initialization by "sgenrand()" is an example. Theoretically,
   there are 2^19937-1 possible states as an intial state.
   Essential bits in "seed_array[]" is following 19937 bits:
    (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1].
   (seed_array[0]&LOWER_MASK) is discarded.
   Theoretically,
    (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]
   can take any values except all zeros.                             */

static double MT_genrand(void)
{
    Int32 *mt = get_RNG_state() + 1;
    Int32 y;
    static Int32 mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    mti = statePointer[0];

    if (mti >= N) { /* generate N words at one time */
	int kk;

	if (mti == N+1)   /* if sgenrand() has not been called, */
	    MT_sgenrand(4357); /* a default initial seed is used   */

	for (kk = 0; kk < N - M; kk++) {
	    y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
	    mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
	}
	for (; kk < N - 1; kk++) {
	    y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
	    mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
	}
	y = (mt[N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
	mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

	mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);
    statePointer[0] = mti;

    return ( (double)y * 2.3283064365386963e-10 ); /* reals: [0,1)-interval */
}

/* =====================================================*/
// from /src/main/times.c - R Project

unsigned int TimeToSeed(void)
{
    unsigned int seed, pid = getpid();
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_REALTIME)
    {
	struct timespec tp;
	clock_gettime(CLOCK_REALTIME, &tp);
	seed = (unsigned int)(((uint_least64_t) tp.tv_nsec << 16) ^ tp.tv_sec);
    }
#elif defined(HAVE_GETTIMEOFDAY)
    {
	struct timeval tv;
	gettimeofday (&tv, NULL);
	seed = (unsigned int)(((uint_least64_t) tv.tv_usec << 16) ^ tv.tv_sec);
    }
#else
    /* C89, so must work */
    seed = (Int32) time(NULL);
#endif
    seed ^= (pid <<16);
    return seed;
}