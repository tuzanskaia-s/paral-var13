#include <mpi.h>
#include <iostream>

#define NUM_COMPUTERS 100

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NUM_COMPUTERS + 2) {
        if (rank == 0) {
            std::cerr << "Test failed: Incorrect number of processes. "
                      << "Expected " << NUM_COMPUTERS + 2 << ", got " << size << ".\n";
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        std::cout << "Test passed: Correct number of processes.\n";
    }

    MPI_Finalize();
    return 0;
}
