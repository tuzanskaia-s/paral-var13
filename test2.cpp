#include <mpi.h>
#include <iostream>

#define SPAMMER_RANK 1
#define NUM_COMPUTERS 100

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int spam_message = 0;

    if (rank == SPAMMER_RANK) {
        spam_message = 42; // Спам-сообщение
        MPI_Bcast(&spam_message, 1, MPI_INT, SPAMMER_RANK, MPI_COMM_WORLD);
        std::cout << "Spammer sent message: " << spam_message << "\n";
    } else if (rank > SPAMMER_RANK && rank <= NUM_COMPUTERS) {
        MPI_Bcast(&spam_message, 1, MPI_INT, SPAMMER_RANK, MPI_COMM_WORLD);
        std::cout << "User " << rank << " received spam message: " << spam_message << "\n";
    }

    MPI_Finalize();
    return 0;
}
