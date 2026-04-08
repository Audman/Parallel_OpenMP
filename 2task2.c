#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <omp.h>

#define N 50'000'000   // 50 million elements

int main()
{
    double *A = (double*)malloc(N * sizeof(double));
    if (A == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand(42);
    for (int i = 0; i < N; i++)
        A[i] = (double)rand() / RAND_MAX;

    double min_diff = DBL_MAX;
    double start = omp_get_wtime();

    #pragma omp parallel for reduction(min:min_diff)
    for (int i = 1; i < N; i++)
    {
        double diff = A[i] - A[i-1];

        if (diff < 0)
            diff = -diff;

        if (diff < min_diff)
            min_diff = diff;
    }

    double end = omp_get_wtime();

    printf("Min |diff| = %.10g\n\n", min_diff);
    printf("Time taken: %f sec\n", end - start);

    free(A);
    return 0;
}
