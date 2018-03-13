cimport cstatr
import numpy as np
cimport numpy as np

def runif(n, min=0.0, max=1.0):
    if n == 0:
        return np.empty()

    cdef arr = np.empty(n)
    cdef arr_min = np.array(min)
    cdef arr_max = np.array(max)

    #cdef np.ndarray[np.double, ndim=1, mode = 'c'] arr_buff = np.ascontiguousarray(np.empty(n), dtype = float)
    #cdef double* arr_ptr = <double*> arr_buff.data  

    cdef double[::1] a = np.empty(n)
    cdef double[::1] b = np.array(min)
    cdef double[::1] c = np.array(max)
    
    cstatr.runif(&a[0], n, &b[0], b.size, &c[0], c.size)
    #cstatr.runif(arr_ptr, n, arr_min, arr_min.size, arr_max, arr_max.size)
    return arr
    
