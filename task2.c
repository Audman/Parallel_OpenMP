#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_ORDERS 100000
#define NUM_THREADS 12

typedef struct {
    int order_id;
    double distance_km;
    int priority;   // 0 = NORMAL, 1 = HIGH
} Order;

int main()
{
    Order *orders = (Order*)malloc(NUM_ORDERS * sizeof(Order));
    if (orders == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int thread_high_count[NUM_THREADS];
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        srand(42);
        for (int i = 0; i < NUM_ORDERS; i++)
        {
            orders[i].order_id = i + 1;
            orders[i].distance_km = (rand() % 5000) / 100.0; // d \in [0,50)
            orders[i].priority = 0;
        }

        static double threshold;
        #pragma omp single
        {
            threshold = 20.0;
        }

        #pragma omp for
        for (int i = 0; i < NUM_ORDERS; i++)
            orders[i].priority = (orders[i].distance_km < threshold);

        #pragma omp barrier


        #pragma omp single
        {
            printf("Priority assignment finished.\n");
        }

        int local_high_count = 0;
        #pragma omp for
        for (int i = 0; i < NUM_ORDERS; i++)
            if (orders[i].priority == 1)
                local_high_count++;

        thread_high_count[thread_id] = local_high_count;

        #pragma omp barrier


        #pragma omp single
        {
            int total_high = 0;
            printf("\nHIGH priority orders per thread:\n");
            for (int t = 0; t < NUM_THREADS; t++)
            {
                printf("Thread %d: %d\n", t, thread_high_count[t]);
                total_high += thread_high_count[t];
            }
            printf("\nTotal HIGH priority orders: %d/%d\n", total_high, NUM_ORDERS);
        }
    }

    free(orders);
    return 0;
}
