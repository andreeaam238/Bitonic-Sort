#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>

#define MASTER 0

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

// Function for computing the minimum of two numbers
int min(int x, int y)
{
    return x < y ? x : y;
}

// Function for initialising an array of a specific length with random numbers
void init_array(int *arr, int length)
{
    for (int i = 0; i < length; ++i)
    {
        arr[i] = rand() % length;
    }
}

// Qsort Comparing function
int cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Function for checking if two arrays are equal
int check_equal_arrays(int *arr1, int *arr2, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (arr1[i] != arr2[i])
        {
            return 1;
        }
    }

    return 0;
}

void bitonic_sort(int length)
{
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // MPI Shared Window
    MPI_Win win;
    MPI_Aint asize;

    int *arr;
    int *arr_sol;
    int start, end;

    // Allocate MPI Window for array
    if (rank == MASTER)
    {
        MPI_Win_allocate_shared(length * sizeof(int), sizeof(int), MPI_INFO_NULL,
                                comm, &arr, &win);
    }
    else
    {
        int disp_unit;
        MPI_Win_allocate_shared(0, sizeof(int), MPI_INFO_NULL,
                                comm, &arr, &win);
        MPI_Win_shared_query(win, 0, &asize, &disp_unit, &arr);
    }

    if (rank == MASTER)
    {
        // Initialise the array
        init_array(arr, length);

        printf("\nInitial array: \n");
        print_array(arr, length);

        arr_sol = (int *)calloc(length, sizeof(int));
        memcpy(arr_sol, arr, length * sizeof(int));

        // Split the array between Workers
        for (int i = size - 1; i > 0; --i)
        {
            start = i * (double)length / size;
            end = min((i + 1) * (double)length / size, length);

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        start = 0;
        end = min((double)length / size, length);
    }
    else
    {
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(comm);

    // i is doubled every iteration
    for (int i = 2; i <= length; i *= 2)
    {
        // j is halved at every iteration
        for (int j = i / 2; j > 0; j /= 2)
        {
            // Sort elements in the thread's subsection
            for (int k = start; k < end; ++k)
            {
                int l = j ^ k;

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
            MPI_Barrier(comm);
        }
    }

    MPI_Barrier(comm);

    if (rank == MASTER)
    {
        printf("\nSorted array: \n");
        print_array(arr, length);

        qsort(arr_sol, length, sizeof(int), cmp);

        if (check_equal_arrays(arr, arr_sol, length))
        {
            printf("\nThe array is not sorted correctly\n");
        }
        else
        {
            printf("\nThe array is sorted correctly\n");
        }

        free(arr_sol);
    }

    MPI_Win_free(&win);

    MPI_Finalize();
}

// Function for checking if a number is a power of two
int is_pow_of_two(int x)
{
    return (ceil(log2(x)) == floor(log2(x)));
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: mpirun -np P %s N\n", argv[0]);
        return 0;
    }

    MPI_Init(&argc, &argv);

    int N = atoi(argv[1]);
    if (!is_pow_of_two(N))
    {
        printf("N has to be a power of two\n");
        MPI_Finalize();
        return 0;
    }

    srand(time(0));

    bitonic_sort(N);

    return 0;
}