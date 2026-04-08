#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100'000'000
#define BINS 256

int main()
{
    unsigned char *A = (unsigned char*)malloc(N * sizeof(unsigned char));
    if (A == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    srand(42);
    for (int i = 0; i < N; i++)
        A[i] = rand() % BINS;
    printf("Init complete\n\n");

    int hist_naive[BINS] = {0};
    double start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < N; i++)
        hist_naive[A[i]]++;

    double end = omp_get_wtime();
    printf("Naive parallel:   %f sec\n", end - start);

    int hist_critical[BINS] = {0};
    start = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < N; i++)
        #pragma omp critical
        {
            hist_critical[A[i]]++;
        }

    end = omp_get_wtime();
    printf("Critical section: %f sec\n", end - start);

    int hist_reduction[BINS] = {0};
    start = omp_get_wtime();

    #pragma omp parallel for reduction(+:hist_reduction[:BINS])
    for (int i = 0; i < N; i++)
        hist_reduction[A[i]]++;

    end = omp_get_wtime();
    printf("Array reduction:  %f sec\n\n\n", end - start);

    long long sum_naive = 0, sum_critical = 0, sum_reduction = 0;
    for (int i = 0; i < BINS; i++)
    {
        sum_naive    += hist_naive[i];
        sum_critical += hist_critical[i];
        sum_reduction+= hist_reduction[i];
    }

    printf("Naive       : %lld\n", sum_naive);
    printf("Critical    : %lld\n", sum_critical);
    printf("Reduction   : %lld\n", sum_reduction);

    free(A);
    return 0;
}
