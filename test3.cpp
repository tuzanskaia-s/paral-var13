#include <mpi.h>
#include <iostream>

#define SERVER_RANK 0
#define TAG_SPAM 101
#define NUM_COMPUTERS 100

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank > 1 && rank <= NUM_COMPUTERS) { // Пользовательские компьютеры
        int spam_message = 1;
        MPI_Send(&spam_message, 1, MPI_INT, SERVER_RANK, TAG_SPAM, MPI_COMM_WORLD);
        std::cout << "User " << rank << " sent spam message to server.\n";
    } else if (rank == SERVER_RANK) { // Сервер
        int received_messages = 0;
        int buffer;
        MPI_Status status;

        while (received_messages < NUM_COMPUTERS) {
            MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, TAG_SPAM, MPI_COMM_WORLD, &status);
            received_messages++;
            std::cout << "Server received spam message " << received_messages
                      << " from user " << status.MPI_SOURCE << ".\n";
        }
    }

    MPI_Finalize();
    return 0;
}
