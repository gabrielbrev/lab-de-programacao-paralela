#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    const int TAMANHO_VETOR = 10;
    int vetor_global[TAMANHO_VETOR];
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int elementos_por_proc = TAMANHO_VETOR / num_procs;
    int vetor_local[elementos_por_proc];

    // Mestre (rank 0) inicializa o vetor completo.
    if (my_rank == 0) {
        for (int i = 0; i < TAMANHO_VETOR; i++) {
            vetor_global[i] = i + 1;
        }
    }

    start_time = MPI_Wtime();

    // 1. Distribuição de dados (Scatter)
    // O mestre (rank 0) envia uma porção do `vetor_global` para cada processo.
    MPI_Scatter(vetor_global, elementos_por_proc, MPI_INT, vetor_local, elementos_por_proc, MPI_INT, 0, MPI_COMM_WORLD);

    int soma_local = 0;
    long long mult_local = 1;
    
    // Realizar somas
    if (my_rank != 0) {
        MPI_Recv(&soma_local, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < elementos_por_proc; i++) {
        soma_local += vetor_local[i];
    }

    if (my_rank < num_procs) {
        MPI_Send(&soma_local, 1, MPI_INT, (my_rank + 1) % num_procs, 0, MPI_COMM_WORLD);
    }

    // Realizar multiplicacoes
    if (my_rank != 0) {
        MPI_Recv(&mult_local, 1, MPI_LONG_LONG, my_rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < elementos_por_proc; i++) {
        mult_local *= vetor_local[i];
    }

    if (my_rank < num_procs ) {
        MPI_Send(&mult_local, 1, MPI_LONG_LONG, (my_rank + 1) % num_procs, 1, MPI_COMM_WORLD);
    }

    if (my_rank == 0) {
        int soma_result;
        long long mult_result;

        MPI_Recv(&soma_result, 1, MPI_INT, num_procs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Resultado da soma: %d\n", soma_result);

        MPI_Recv(&mult_result, 1, MPI_LONG_LONG, num_procs - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Resultado da multiplicação: %lld\n", mult_result);
    }

    MPI_Finalize();
    return(0);
}

