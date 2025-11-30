#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 1000
#define NUM_EXECUTIONS 10

void initialize_matrix(double **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = 1.0;
        }
    }
}

double** allocate_matrix(int size) {
    double **matrix = (double **)malloc(size * sizeof(double *));
    for (int i = 0; i < size; i++) {
        matrix[i] = (double *)malloc(size * sizeof(double));
    }
    return matrix;
}

void free_matrix(double **matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void matrix_multiply(double **A, double **B, double **C, int size, int num_threads, char *schedule_type) {
    omp_set_num_threads(num_threads);
    
    if (strcmp(schedule_type, "static") == 0) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < size; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    } else if (strcmp(schedule_type, "dynamic") == 0) {
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < size; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    } else if (strcmp(schedule_type, "guided") == 0) {
        #pragma omp parallel for schedule(guided)
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                C[i][j] = 0.0;
                for (int k = 0; k < size; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
}

// Executa as multiplicacoes N vezes (NUM_EXECUCOES) e devolve a media de tempos de execucao
double run_test(double **A, double **B, double **C, int size, int num_threads, char *schedule_type) {
    double total_time = 0.0;
    
    for (int exec = 0; exec < NUM_EXECUTIONS; exec++) {
        double start_time = omp_get_wtime();
        matrix_multiply(A, B, C, size, num_threads, schedule_type);
        double end_time = omp_get_wtime();
        total_time += (end_time - start_time);
    }
    
    return total_time / NUM_EXECUTIONS;
}

int main() {
    double **A = allocate_matrix(N);
    double **B = allocate_matrix(N);
    double **C = allocate_matrix(N);
    
    initialize_matrix(A, N);
    initialize_matrix(B, N);
    
    int thread_counts[] = {2, 4, 8, 16};
    int num_thread_tests = 4;
    
    char *schedule_types[] = {"static", "dynamic", "guided"};
    int num_schedule_types = 3;
    
    printf("=================================================================\n");
    printf("Multiplicação de Matrizes Paralela com OpenMP\n");
    printf("Tamanho das matrizes: %d x %d\n", N, N);
    printf("Número de execuções por teste: %d\n", NUM_EXECUTIONS);
    printf("=================================================================\n\n");
    
    for (int t = 0; t < num_thread_tests; t++) {
        int num_threads = thread_counts[t];
        
        printf("-----------------------------------------------------------------\n");
        printf("Testes com %d threads:\n", num_threads);
        printf("-----------------------------------------------------------------\n");
        
        for (int s = 0; s < num_schedule_types; s++) {
            char *schedule_type = schedule_types[s];
            
            double avg_time = run_test(A, B, C, N, num_threads, schedule_type);
            
            printf("  Escalonamento %-10s: %.6f segundos (média de %d execuções)\n", 
                   schedule_type, avg_time, NUM_EXECUTIONS);
        }
        printf("\n");
    }
    
    printf("=================================================================\n");
    printf("Verificação do resultado (amostra):\n");
    printf("C[0][0] = %.0f (esperado: %d)\n", C[0][0], N);
    printf("C[%d][%d] = %.0f (esperado: %d)\n", N-1, N-1, C[N-1][N-1], N);
    printf("=================================================================\n");
    
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);
    
    return 0;
}
