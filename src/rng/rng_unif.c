/*
 * statr
 * Copyright (C) 2018 Piotr Krzeszewski
 * 
 * R : A Computer Language for Statistical Data Analysis
 * Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka
 * Copyright (C) 1997--2016  The R Core Team
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

#include "../rng.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h> /* for getpid in TimeToSeed*/
#include <time.h> /* for time() in TimeToSeed */

#define MT_STATE_SIZE 625
#define i2_32m1 2.328306437080797e-10/* = 1/(2^32 - 1) */

// Mersenne Twister defines moved up
#define N 624
// end

RNG_state_t* create_RNG_state(unsigned int seed)
{
    RNG_state_t *state = (RNG_state_t *)malloc(sizeof(RNG_state_t));
    state->seed = seed;
    return state;
}

void delete_RNG_state(RNG_state_t *state)
{
    if (state == NULL)
        return;
    if (state->state != NULL) 
        free(state->state);
    free(state);
}

static void RNG_Init(RNG_state_t *state);
static void Randomize(RNG_state_t *state);
unsigned int TimeToSeed(void);

static double MT_genrand(RNG_state_t *state);


static double fixup(double x)
{
    /* ensure 0 and 1 are never returned */
    if(x <= 0.0) 
        return 0.5*i2_32m1;
    if((1.0 - x) <= 0.0) 
        return 1.0 - 0.5*i2_32m1;
    return x;
}

double unif_rand(RNG_state_t **state)
{
    if (state == NULL) {
        state = (RNG_state_t **)malloc(sizeof(RNG_state_t *));
    }
    if (*state == NULL) {
        *state = (RNG_state_t *)malloc(sizeof(RNG_state_t));
        (*state)->seed = TimeToSeed();
        (*state)->state = NULL;
        (*state)->MT_index = N + 1;
    }
    if ((*state)->state == NULL) {
        (*state)->state = (unsigned int*)malloc(MT_STATE_SIZE * sizeof(unsigned int));
        (*state)->seed = (*state)->seed != 0 ? (*state)->seed : TimeToSeed();
        RNG_Init(*state);
    }

	return fixup(MT_genrand(*state));
}

static void FixupSeeds(RNG_state_t *state, int initial)
{
    int j, notallzero = 0;

	if(initial) {
        state->state[0] = 624;
    }
	 /* No action unless user has corrupted .Random.seed */
	if( state->state[0] <= 0) {
        state->state[0] = 624;
    }
	/* check for all zeroes */
	for (j = 1; j <= 624; j++) {
	    if (state->state[j] != 0) {
            notallzero = 1;
            break;
        }
	}
	if(!notallzero) { 
        Randomize(state);
    }
}

static void RNG_Init(RNG_state_t *state)
{
    int j;
    unsigned int seed = state->seed;
    /* Initial scrambling */
    for(j = 0; j < 50; j++) {
	    seed = (69069 * seed + 1);
    }

	/* i_seed[0] is mti, *but* this is needed for historical consistency */
	for(j = 0; j < 625; j++) {
	    seed = (69069 * seed + 1);
	    state->state[j] = seed;
	}
	FixupSeeds(state, 1);
}

// This method will be called in very unlikely case, when state will be generated incorrectly.
static void Randomize(RNG_state_t *state)
{
    state->seed = TimeToSeed();
    RNG_Init(state);
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

/* Initializing the array with a seed */
static void MT_sgenrand(RNG_state_t *state, unsigned int internal_seed)
{
    unsigned int *mt = state->state + 1;
    int i;

    for (i = 0; i < N; i++) {
        mt[i] = internal_seed & 0xffff0000;
        internal_seed = 69069 * internal_seed + 1;
        mt[i] |= (internal_seed & 0xffff0000) >> 16;
        internal_seed = 69069 * internal_seed + 1;
    }
    state->MT_index = N;
}

/* Initialization by "sgenrand()" is an example. Theoretically,
   there are 2^19937-1 possible states as an intial state.
   Essential bits in "seed_array[]" is following 19937 bits:
    (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1].
   (seed_array[0]&LOWER_MASK) is discarded.
   Theoretically,
    (seed_array[0]&UPPER_MASK), seed_array[1], ..., seed_array[N-1]
   can take any values except all zeros.                             */

static double MT_genrand(RNG_state_t *state)
{
    unsigned int *mt = state->state + 1;
    unsigned int y;
    unsigned int mag01[2] = {0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    int mti = state->state[0];

    if (mti >= N) { /* generate N words at one time */
	int kk;

	if (mti == N+1)   /* if sgenrand() has not been called, */
	    MT_sgenrand(state, 4357); /* a default initial seed is used   */

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
    state->state[0] = mti;

    state->MT_index = mti;

    return ( (double)y * 2.3283064365386963e-10 ); /* reals: [0,1)-interval */
}

/* =====================================================*/
// from /src/main/times.c - R Project

// TODO: Probably in future something like autoheader should be added to properlu use other methods of time generation
unsigned int TimeToSeed(void)
{
    unsigned int seed, pid = getpid();
// #if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_REALTIME)
//     {
// 	struct timespec tp;
// 	clock_gettime(CLOCK_REALTIME, &tp);
// 	seed = (unsigned int)(((uint_least64_t) tp.tv_nsec << 16) ^ tp.tv_sec);
//     }
// #elif defined(HAVE_GETTIMEOFDAY)
//     {
// 	struct timeval tv;
// 	gettimeofday (&tv, NULL);
// 	seed = (unsigned int)(((uint_least64_t) tv.tv_usec << 16) ^ tv.tv_sec);
//     }
// #else
    /* C89, so must work */
    seed = (unsigned int) time(NULL);
//#endif
    seed ^= (pid <<16);
    return seed;
}