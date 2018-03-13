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

#include "distr/distr.h"
#include <stdio.h>

double runif_single(double a, double b);

void runif(double *res, int nres, double *a, int na, double *b, int nb)
{
    if (nres == 0) {
        return;
    }
    if (na < 1 || nb < 1) {
        fillWithNAN(res, nres);
        return;
    }
    else {
        int i;
        for (i = 0; i < nres; i++) {
            res[i] = runif_single(a[i % na], b[i % nb]);
        }
    }
}

double runif_single(double a, double b)
{
    if (a == b)
	    return a;
    else {
        double u;
        do {
            u = unif_rand();
        } while (u <= 0 || u >= 1);
        return a + (b - a) * u;
    }
}
