#include <mpi.h>
#include <iostream>
#include <vector>

#define NUM_COMPUTERS 100  // Number of user computers
#define TAG_SPAM 101       // Tag for spam messages
#define TAG_REVENGE 111    // Tag for revenge messages
#define SERVER_RANK 0      // Hexagon server rank
#define SPAMMER_RANK 1     // Spammer rank
#define REVENGE_THRESHOLD 20 // Number of messages to trigger revenge

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of processes

    if (size != NUM_COMPUTERS + 2) {
        if (rank == SERVER_RANK) {
            std::cerr << "This program requires exactly " << NUM_COMPUTERS + 2
                << " MPI processes: 100 user computers, 1 spammer, 1 server.\n";
        }
        MPI_Finalize();
        return 1;
    }

    if (rank == SPAMMER_RANK) {
        // Spammer process
        std::cout << "Spammer: Sending spam messages to all user computers...\n";
        int spam_message = 1; // Represents the spam message
        MPI_Bcast(&spam_message, 1, MPI_INT, SPAMMER_RANK, MPI_COMM_WORLD); // Broadcast to all users
    }
    else if (rank > SPAMMER_RANK && rank <= NUM_COMPUTERS) {
        // User computer processes
        int received_message;
        MPI_Bcast(&received_message, 1, MPI_INT, SPAMMER_RANK, MPI_COMM_WORLD); // Receive spam
        std::cout << "User computer " << rank << ": Received spam, forwarding to server...\n";
        MPI_Send(&received_message, 1, MPI_INT, SERVER_RANK, TAG_SPAM, MPI_COMM_WORLD); // Forward to server

        // Wait for potential revenge signal from server
        int revenge_signal = 0;
        MPI_Bcast(&revenge_signal, 1, MPI_INT, SERVER_RANK, MPI_COMM_WORLD); // Receive revenge signal
        if (revenge_signal == TAG_REVENGE) {
            std::cout << "User computer " << rank << ": Received revenge signal, retaliating against spammer...\n";
            int revenge_message = 11; // Represents the "Revenge" message
            for (int i = 0; i < 11; i++) {
                MPI_Send(&revenge_message, 1, MPI_INT, SPAMMER_RANK, TAG_REVENGE, MPI_COMM_WORLD);
            }
        }
    }
    else if (rank == SERVER_RANK) {
        // Hexagon server process
        int message_count = 0;
        int buffer;
        MPI_Status status;

        std::cout << "Server: Waiting for spam messages...\n";

        while (message_count < REVENGE_THRESHOLD) {
            MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, TAG_SPAM, MPI_COMM_WORLD, &status);
            message_count++;
            std::cout << "Server: Received spam message " << message_count << " from user computer "
                << status.MPI_SOURCE << ".\n";
        }

        // Revenge triggered
        std::cout << "Server: Revenge triggered. Broadcasting revenge signal to all user computers...\n";
        int revenge_signal = TAG_REVENGE;
        MPI_Bcast(&revenge_signal, 1, MPI_INT, SERVER_RANK, MPI_COMM_WORLD); // Broadcast revenge signal
    }

    MPI_Finalize();
    return 0;
}
