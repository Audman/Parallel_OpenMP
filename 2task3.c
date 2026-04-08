#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100'000'000

int main()
{
    double *A = (double*)malloc(N * sizeof(double));
    if (A == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Init... \n");
    srand(42);
    for (int i = 0; i < N; i++)
        A[i] = (double)rand() / RAND_MAX;
    printf("Init complete\n\n");

    double max_val = 0.0;
    double start_max = omp_get_wtime();

    #pragma omp parallel for reduction(max:max_val)
    for (int i = 0; i < N; i++)
        if (A[i] > max_val) max_val = A[i];

    double end_max = omp_get_wtime();
    printf("Max reduction:   %f seconds\n", end_max - start_max);

    double T = 0.8 * max_val;

    double sum_above = 0.0;
    double start_sum = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum_above)
    for (int i = 0; i < N; i++)
        if (A[i] > T) sum_above += A[i];

    double end_sum = omp_get_wtime();
    printf("Sum reduction:   %f seconds\n", end_sum - start_sum);

    printf("\n\n");
    printf("Maximum value:         %.10f\n", max_val);
    printf("Threshold T = 0.8*max: %.10f\n", T);
    printf("Sum of elements > T:   %.10f\n", sum_above);

    free(A);
    return 0;
}
