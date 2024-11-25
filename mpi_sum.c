/*Design a Distributed Application using Message Passing
Interface (MPI) for remote computation.*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 1000             // Size of the array
#define CHUNK_SIZE (ARRAY_SIZE / 4) // Number of elements per process (assuming 4 processes)

int main(int argc, char *argv[]) {
    int rank, size;
    int *array = NULL;
    int local_sum = 0, global_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ensure we have exactly 4 processes
    if (size != 4) {
        if (rank == 0) {
            fprintf(stderr, "This program requires exactly 4 processes.\n");
        }
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Initialize array only on root process (rank 0)
    if (rank == 0) {
        array = (int *)malloc(ARRAY_SIZE * sizeof(int));
        if (array == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        for (int i = 0; i < ARRAY_SIZE; i++) {
            array[i] = i + 1; // Initialize array with values 1, 2, ..., ARRAY_SIZE
        }
    }

    // Allocate memory for local computation
    int local_array[CHUNK_SIZE];

    // Scatter the array to all processes
    MPI_Scatter(array, CHUNK_SIZE, MPI_INT, local_array, CHUNK_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute the local sum
    for (int i = 0; i < CHUNK_SIZE; i++) {
        local_sum += local_array[i];
    }

    // Reduce the local sums to compute the global sum
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print the result only on root process (rank 0)
    if (rank == 0) {
        printf("The total sum of the array is %d\n", global_sum);
        free(array);
    }

    MPI_Finalize();
    return 0;
}