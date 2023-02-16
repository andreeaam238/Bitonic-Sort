#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

// Thread Task
typedef struct
{
    int *arr;
    int length;
    int start;
    int end;
    pthread_barrier_t *barrier;
} Task;

// Function for printing an array
void print_array(int *arr, int n)
{
    for (int i = 0; i < n; ++i)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Function for swapping two numbers
void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

// Function for computing the minimum of two numbers
int min(int x, int y)
{
    return x < y ? x : y;
}

// Thread Function
void *bitonic_sort(void *arg)
{
    // Get Thread Info
    Task *task = (Task *)arg;
    int *arr = task->arr;
    int length = task->length;
    int start = task->start;
    int end = task->end;
    pthread_barrier_t *barrier = task->barrier;

    int l;

    // i is doubled every iteration
    for (int i = 2; i <= length; i *= 2)
    {
        // j is halved at every iteration
        for (int j = i / 2; j > 0; j /= 2)
        {
            // Sort elements in the thread's subsection
            for (int k = start; k < end; ++k)
            {
                l = j ^ k;
                if (l > k)
                {
                    // Swap elements
                    if ((i & k) == 0 && arr[k] > arr[l])
                    {
                        swap(&arr[k], &arr[l]);
                    }
                    else if ((i & k) != 0 && arr[k] < arr[l])
                    {
                        swap(&arr[k], &arr[l]);
                    }
                }
            }
            pthread_barrier_wait(barrier);
        }
    }

    pthread_exit(NULL);
}

void sort(int *arr, int N, int P)
{
    printf("\nInitial array: \n");
    print_array(arr, N);

    pthread_t threads[P];
    Task arguments[P];

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, P);

    // Start the Threads and split the array between them
    for (int id = 0; id < P; id++)
    {
        arguments[id].arr = arr;
        arguments[id].length = N;
        arguments[id].start = id * (double)N / P;
        arguments[id].end = min((id + 1) * (double)N / P, N);
        arguments[id].barrier = &barrier;

        pthread_create(&threads[id], NULL, bitonic_sort, (void *)&arguments[id]);
    }

    // Wait for all Threads to finish
    for (int id = 0; id < P; id++)
    {
        pthread_join(threads[id], NULL);
    }

    printf("\nSorted array: \n");
    print_array(arr, N);
}

// Function for initialising an array of a specific length with random numbers
int *init_array(int length)
{
    int *arr = calloc(length, sizeof(int));
    if (!arr)
    {
        return NULL;
    }

    for (int i = 0; i < length; ++i)
    {
        arr[i] = rand() % length;
    }

    return arr;
}

// Qsort Comparing function
int cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Function for checking if two arrays are equal
int check_equal_arrays(int *arr1, int *arr2, int length)
{
    for (int i = 0; i < length; ++i)
    {
        if (arr1[i] != arr2[i])
        {
            return 1;
        }
    }

    return 0;
}

// Function for checking if a number is a power of two
int is_pow_of_two(int x)
{
    return (ceil(log2(x)) == floor(log2(x)));
}

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s N P\n", argv[0]);
        return 0;
    }

    int N = atoi(argv[1]);
    if (!is_pow_of_two(N))
    {
        printf("N has to be a power of two\n");
        return 0;
    }

    int P = atoi(argv[2]);

    srand(time(0));
    int *arr = init_array(N);
    
    int *arr_sol = calloc(N, sizeof(int));
    memcpy(arr_sol, arr, N * sizeof(int));

    sort(arr, N, P);
    qsort(arr_sol, N, sizeof(int), cmp);

    if (check_equal_arrays(arr, arr_sol, N))
    {
        printf("\nThe array is not sorted correctly\n");
        return 0;
    }

    printf("\nThe array is sorted correctly\n");

    free(arr);
    free(arr_sol);

    return 0;
}
