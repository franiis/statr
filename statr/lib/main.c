#include "statr.h"
#include "prech.h"

int main()
{
    int i;
    set_seed(10);

    double *first = (double*)malloc(sizeof(double) * 10);
    double *first_a = (double*)malloc(sizeof(double));
    double *first_b = (double*)malloc(sizeof(double));
    *first_a = 0;
    *first_b = 1;
    runif(first, 10, first_a, 1, first_b, 1);
    for (i = 0; i < 10; i++) {
        printf("Liczba: %f\n", first[i]);
    }
    free(first);
    free(first_a);
    free(first_b);

    set_seed(20);
    double *second = (double*)malloc(sizeof(double) * 10);
    double *second_a = (double*)malloc(sizeof(double) * 2);
    double *second_b = (double*)malloc(sizeof(double) * 2);
    second_a[0] = 0;
    second_a[1] = 2;
    second_b[0] = 1;
    second_b[1] = 3;
    runif(second, 10, second_a, 2, second_b, 2);
    for (i = 0; i < 10; i++) {
        printf("Liczba: %f\n", second[i]);
    }
    free(second);
    free(second_a);
    free(second_b);
    return EXIT_SUCCESS;
}