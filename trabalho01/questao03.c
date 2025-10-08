#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[]) {
    int my_rank, num_procs;
    const int TAMANHO_VETOR = 12;
    int vetor_global[TAMANHO_VETOR];
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // O tamanho do vetor deve ser divisível pelo número de processos para este exemplo simples.
    if (TAMANHO_VETOR % num_procs != 0) {
        if (my_rank == 0) {
            printf("Erro: O tamanho do vetor (%d) deve ser divisivel pelo numero de processos (%d).\n", TAMANHO_VETOR, num_procs);
        }
        MPI_Finalize();
        return 1;
    }

    int elementos_por_proc = TAMANHO_VETOR / num_procs;
    int vetor_local[elementos_por_proc];

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
    int sub_local = 0;
    long long mult_local = 1;

    // ----- SOMA -----

    // Caso não seja o primeiro processo, deve esperar a soma do ultimo, caso contrario inicia a operacao de soma
    if (my_rank != 0) {
        MPI_Recv(&soma_local, 1, MPI_INT, my_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < elementos_por_proc; i++) {
        soma_local += vetor_local[i];
    }

    // Caso nao seja o ultimo processo, envia para o proximo. Caso contrario, envia para o rank 0
    if (my_rank < num_procs - 1) {
        MPI_Send(&soma_local, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(&soma_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // ----- SUBTRACAO -----

    // Caso não seja o primeiro processo, deve esperar a soma do ultimo, caso contrario inicia a operacao de soma
    if (my_rank != 0) {
        MPI_Recv(&sub_local, 1, MPI_INT, my_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < elementos_por_proc; i++) {
        sub_local -= vetor_local[i];
    }

    // Caso nao seja o ultimo processo, envia para o proximo. Caso contrario, envia para o rank 0
    if (my_rank < num_procs - 1) {
        MPI_Send(&sub_local, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(&sub_local, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // ----- MULTIPLICACAO -----
    
    // Caso não seja o primeiro processo, deve esperar a soma do ultimo, caso contrario inicia a operacao de multiplicacao
    if (my_rank != 0) {
        MPI_Recv(&mult_local, 1, MPI_LONG_LONG, my_rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < elementos_por_proc; i++) {
        mult_local *= vetor_local[i];
    }

    // Caso nao seja o ultimo processo, envia para o proximo. Caso contrario, envia para o rank 0
    if (my_rank < num_procs - 1) {
        MPI_Send(&mult_local, 1, MPI_LONG_LONG, my_rank + 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(&mult_local, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0) {
        int soma_result;
        int sub_result;
        long long mult_result;

        MPI_Recv(&soma_result, 1, MPI_INT, num_procs - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&sub_result, 1, MPI_INT, num_procs - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mult_result, 1, MPI_LONG_LONG, num_procs - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        end_time = MPI_Wtime();

        printf("\nResultados Finais:\n");
        printf("Soma = %d\n", soma_result);
        printf("Subtracao = %d\n", sub_result);
        printf("Multiplicacao = %lld\n", mult_result);
        printf("Tempo de Execucao: %f segundos\n", end_time - start_time);
    }

    MPI_Finalize();
    return(0);
}

