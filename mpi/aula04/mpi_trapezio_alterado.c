#include <stdio.h>
#include <math.h>
#include "mpi.h"

double f(double x) {
    double return_val;
    return_val = x * x; /* Função exponencial */
    return return_val;
}

int main(int argc, char *argv[]) { /* mpi_trapezio.c  */
    int meu_ranque, num_procs;  /* respectivamente q e p */
    double a = 0.0,  b = 1.0;   /* Limites da integral */
    double tempo_inicial, tempo_final; /* Tempo de execução */
    long int n = 100000000;     /* Número de trapezoides */
    double x, h;                /* x e h, a base do trapezoide */
    double integral=0.0, total; /* Integral de cada processo e total */
    int origem, destino = 0;    /* Origem e destino das mensagens */
    int etiq = 3;               /* Uma etiqueta qualquer */

    /* Inicia o MPI e determina o ranque e o número de processos ativos  */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    /* h é o mesmo para todos os processos */
    h = (b - a)/n;
    long int local_n = n / num_procs; // número de trapézios por processo
    double local_a = a + meu_ranque * local_n * h;
    double local_b = local_a + local_n * h;
    if (meu_ranque == 0) {
        tempo_inicial = MPI_Wtime();
    }
    // Cada processo calcula sua integral local
    double local_integral = (f(local_a) + f(local_b)) / 2.0;
    for (long int i = 1; i < local_n; i++) {
        double x_i = local_a + i * h;
        local_integral += f(x_i);
    }
    local_integral = local_integral * h;
    integral = local_integral;
    /* O processo 0 soma as integrais parciais recebidas */
    if (meu_ranque == 0) { 
        total = integral; 
        for (origem = 1; origem < num_procs; origem++) { 
             MPI_Recv(&integral, 1, MPI_DOUBLE, origem, etiq, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
             total += integral; 
        }
    } 
     /* Os demais processos enviam as integrais parciais para o processo 0 */
    else {
        MPI_Send(&integral, 1, MPI_DOUBLE, destino, etiq, MPI_COMM_WORLD);
    }
    /* Imprime o resultado */
    if (meu_ranque == 0) {
        tempo_final = MPI_Wtime();
        printf("Foram gastos %3.1f segundos\n",tempo_final-tempo_inicial);
        printf("Com n = %ld trapezoides, a estimativa\n", n);
        printf("da integral de %lf até %lf = %lf \n", a, b, total);
    }
    MPI_Finalize();
    return(0);
} 