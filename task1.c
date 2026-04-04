#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_LOGS 200000
#define NUM_THREADS 4

typedef struct {
    int request_id;
    int user_id;
    int response_time_ms;
} LogEntry;

typedef struct {
    int fast;
    int medium;
    int slow;
} ThreadCounts;

int main()
{
    LogEntry *logs = (LogEntry*)malloc(NUM_LOGS * sizeof(LogEntry));
    if (logs == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    ThreadCounts thread_counts[NUM_THREADS];
    omp_set_num_threads(NUM_THREADS);

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        #pragma omp single
        {
            srand(42);
            for (int i = 0; i < NUM_LOGS; i++)
            {
                logs[i].request_id = i + 1;
                logs[i].user_id = rand() % 1000 + 1;
                logs[i].response_time_ms = rand() % 600;
            }
        }

        #pragma omp barrier


        int local_fast = 0, local_medium = 0, local_slow = 0;

        #pragma omp for schedule(static)
        for (int i = 0; i < NUM_LOGS; i++)
        {
            int rt = logs[i].response_time_ms;
            if (rt < 100)
                local_fast++;
            else if (rt <= 300)
                local_medium++;
            else
                local_slow++;
        }

        thread_counts[thread_id].fast   = local_fast;
        thread_counts[thread_id].medium = local_medium;
        thread_counts[thread_id].slow   = local_slow;

        #pragma omp barrier


        #pragma omp single
        {
            int total_fast = 0, total_medium = 0, total_slow = 0;
            for (int t = 0; t < NUM_THREADS; t++)
            {
                total_fast   += thread_counts[t].fast;
                total_medium += thread_counts[t].medium;
                total_slow   += thread_counts[t].slow;
            }

            printf("FAST:   %d\n", total_fast);
            printf("MEDIUM: %d\n", total_medium);
            printf("SLOW:   %d\n\n", total_slow);
            printf("Logs:   %d\n", total_fast + total_medium + total_slow);
        }
    }

    free(logs);
    return 0;
}
