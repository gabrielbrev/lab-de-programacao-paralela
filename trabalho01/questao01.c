#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int my_rank, num_procs;
    const int TAMANHO_VETOR = 12;
    int vet[TAMANHO_VETOR];
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs != 4) {
        if (my_rank == 0) {
            printf("Erro: Esta versao requer exatamente 4 processos.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Processo 0: Prepara os dados, distribui e printa os resultados.
    if (my_rank == 0) {
        int soma_final;
        int subtracao_final;
        long long mult_final;

        for (int i = 0; i < TAMANHO_VETOR; i++) {
            vet[i] = i + 1;
        }

        start_time = MPI_Wtime();

        MPI_Send(vet, TAMANHO_VETOR, MPI_INT, 1, 0, MPI_COMM_WORLD); // Soma
        MPI_Send(vet, TAMANHO_VETOR, MPI_INT, 2, 0, MPI_COMM_WORLD); // Subtração
        MPI_Send(vet, TAMANHO_VETOR, MPI_INT, 3, 0, MPI_COMM_WORLD); // Multiplicação

        MPI_Recv(&soma_final, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&subtracao_final, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&mult_final, 1, MPI_LONG_LONG, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        end_time = MPI_Wtime();

        printf("Resultados Finais:\n");
        printf("Soma = %d\n", soma_final);
        printf("Subtracao = %d\n", subtracao_final);
        printf("Multiplicacao = %lld\n", mult_final);
        printf("Tempo de Execucao: %f segundos\n", end_time - start_time);
    }
    // Processo 1: Calcula a SOMA.
    else if (my_rank == 1) {
        int soma = 0;
        MPI_Recv(vet, TAMANHO_VETOR, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < TAMANHO_VETOR; i++) {
            soma += vet[i];
        }
        MPI_Send(&soma, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    // Processo 2: Calcula a SUBTRAÇÃO.
    else if (my_rank == 2) {
        int subtracao = 0;
        MPI_Recv(vet, TAMANHO_VETOR, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < TAMANHO_VETOR; i++) {
            subtracao -= vet[i];
        }
        MPI_Send(&subtracao, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    // Processo 3: Calcula a MULTIPLICAÇÃO.
    else if (my_rank == 3) {
        long long mult = 1;
        MPI_Recv(vet, TAMANHO_VETOR, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < TAMANHO_VETOR; i++) {
            mult *= vet[i];
        }
        MPI_Send(&mult, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}