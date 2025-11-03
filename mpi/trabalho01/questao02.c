#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int my_rank, num_procs;
    const int TAMANHO_VETOR = 1200;
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

    // Mestre (rank 0) inicializa o vetor completo.
    if (my_rank == 0) {
        for (int i = 0; i < TAMANHO_VETOR; i++) {
            vetor_global[i] = i + 1;
        }
    }

    start_time = MPI_Wtime();

    // 1. Distribuição de dados (Scatter)
    // O mestre (rank 0) envia uma porção do `vetor_global` para cada processo.
    MPI_Scatter(vetor_global, elementos_por_proc, MPI_INT,
                vetor_local, elementos_por_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Computação Local
    // Todos os processos, incluindo o mestre, calculam sobre sua porção local.
    int soma_local = 0;
    long long mult_local = 1;
    for (int i = 0; i < elementos_por_proc; i++) {
        soma_local += vetor_local[i];
        mult_local *= vetor_local[i];
    }

    // 3. Redução (combinação) dos resultados
    int soma_global;
    long long mult_global;
    
    // Combina todas as `soma_local` usando a operação de soma e guarda no rank 0.
    MPI_Reduce(&soma_local, &soma_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    // Combina todas as `mult_local` usando a operação de produto e guarda no rank 0.
    MPI_Reduce(&mult_local, &mult_global, 1, MPI_LONG_LONG, MPI_PROD, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    // Mestre (rank 0) imprime os resultados finais.
    if (my_rank == 0) {
        int subtracao_global = -soma_global; // Solução para a falta de MPI_SUB

        printf("\nResultados Finais:\n");
        printf("Soma = %d\n", soma_global);
        printf("Subtracao = %d\n", subtracao_global);
        printf("Multiplicacao = %lld\n", mult_global);
        printf("Tempo de Execucao: %f segundos\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}