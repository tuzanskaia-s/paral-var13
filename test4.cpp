#include <mpi.h>
#include <iostream>

#define SERVER_RANK 0
#define TAG_SPAM 101
#define TAG_REVENGE 111
#define NUM_COMPUTERS 100
#define REVENGE_THRESHOLD 20

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank > 1 && rank <= NUM_COMPUTERS) { // Пользовательские компьютеры
        int spam_message = 1;
        MPI_Send(&spam_message, 1, MPI_INT, SERVER_RANK, TAG_SPAM, MPI_COMM_WORLD);
    } else if (rank == SERVER_RANK) { // Сервер
        int message_count = 0;
        int buffer;
        MPI_Status status;

        while (message_count < REVENGE_THRESHOLD) {
            MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, TAG_SPAM, MPI_COMM_WORLD, &status);
            message_count++;
        }

        std::cout << "Server: Revenge threshold reached. Broadcasting revenge signal.\n";
        int revenge_signal = TAG_REVENGE;
        MPI_Bcast(&revenge_signal, 1, MPI_INT, SERVER_RANK, MPI_COMM_WORLD);
    } else if (rank > 1 && rank <= NUM_COMPUTERS) { // Пользовательские компьютеры
        int revenge_signal = 0;
        MPI_Bcast(&revenge_signal, 1, MPI_INT, SERVER_RANK, MPI_COMM_WORLD);

        if (revenge_signal == TAG_REVENGE) {
            std::cout << "User " << rank << " received revenge signal.\n";
