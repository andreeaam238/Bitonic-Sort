#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>

// Function for printing an array
void print_array(int *arr, int length)
{
    for (int i = 0; i < length; ++i)
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

// Sort Function
void bitonic_sort(int *arr, int length, int no_threads)
{
    int l;

    // i is doubled every iteration
    for (int i = 2; i <= length; i *= 2)
    {
        // j is halved at every iteration
        for (int j = i / 2; j > 0; j /= 2)
        {
            // Sort elements in the thread's subsection
#pragma omp parallel for private(l) num_threads(no_threads)
            for (int k = 0; k < length; ++k)
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
        }
    }
}

void sort(int *arr, int N, int P)
{
    printf("\nInitial array: \n");
    print_array(arr, N);

    bitonic_sort(arr, N, P);

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
