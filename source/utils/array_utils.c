#include "array_utils.h"
#include "prech.h"
#include "math.h"

void fillWithNAN(double *arr, int arr_len)
{
    if (arr == NULL || arr_len == 0) {
        return;
    }
    int i;
    for (i = 0; i < arr_len; i++) {
        arr[i] = NAN;
    }
}

