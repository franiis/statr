###
# statr
# Copyright (C) 2018 Piotr Krzeszewski
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
###

import ctypes
import os
fp = os.path.dirname(__file__)
_statr = ctypes.CDLL(fp + '/build/statr_lib.so')

_statr.runif.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.c_int, ctypes.POINTER(ctypes.c_double), 
                            ctypes.c_int, ctypes.POINTER(ctypes.c_double), ctypes.c_int]
_statr.runif.restype = None

_statr.set_seed.argtypes = [ctypes.c_int]
_statr.set_seed.restype = None


def runif(n, min=0.0, max=1.0):
    global _statr
    if n == 0:
        return []
    if not isinstance(min, list):
        min = [min]
    if not isinstance(max, list):
        max = [max]
    res = (ctypes.c_double * n)()
    cmin = (ctypes.c_double * len(min))(*min)
    cmax = (ctypes.c_double * len(max))(*max)
    _statr.runif(res, len(res), cmin, len(min), cmax, len(max))
    return list(res)

def set_seed(seed):
    global _statr
    _statr.set_seed(seed)