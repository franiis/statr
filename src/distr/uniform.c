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

#include "../distr.h"
#include "../rng.h"
#include "../utils/array_utils.h"
#include <stdio.h>

double runif(RNG_state_t **state, double a, double b)
{
    if (a == b)
	    return a;
    else {
        double u;
        do {
            u = unif_rand(state);
        } while (u <= 0 || u >= 1);
        return a + (b - a) * u;
    }
}
