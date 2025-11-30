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
    
    // Armazenar resultados para cálculo de speedup e eficiencia
    double results[num_thread_tests][num_schedule_types];
    
    printf("Calculando tempo sequencial (1 thread)\n\n");
    double sequential_time = run_test(A, B, C, N, 1, "static");
    printf("Tempo sequencial: %.6f segundos\n\n", sequential_time);
    
    for (int t = 0; t < num_thread_tests; t++) {
        int num_threads = thread_counts[t];
        
        printf("-----------------------------------------------------------------\n");
        printf("Testes com %d threads:\n", num_threads);
        printf("-----------------------------------------------------------------\n");
        
        for (int s = 0; s < num_schedule_types; s++) {
            char *schedule_type = schedule_types[s];
            
            double avg_time = run_test(A, B, C, N, num_threads, schedule_type);
            results[t][s] = avg_time;
            
            printf("  Escalonamento %-10s: %.6f segundos (média de %d execuções)\n", schedule_type, avg_time, NUM_EXECUTIONS);
        }
        printf("\n");
    }
    
    printf("=================================================================\n");
    printf("ANÁLISE DE DESEMPENHO\n");
    printf("=================================================================\n\n");
    
    printf("Tempo Sequencial (1 thread): %.6f segundos\n\n", sequential_time);
    
    for (int t = 0; t < num_thread_tests; t++) {
        int num_threads = thread_counts[t];
        
        printf("-----------------------------------------------------------------\n");
        printf("Threads: %d\n", num_threads);
        printf("-----------------------------------------------------------------\n");
        printf("%-12s | %-12s | %-10s | %-10s\n", 
               "Escalonamento", "Tempo (s)", "Speedup", "Eficiência");
        printf("-----------------------------------------------------------------\n");
        
        for (int s = 0; s < num_schedule_types; s++) {
            char *schedule_type = schedule_types[s];
            double time = results[t][s];
            double speedup = sequential_time / time;
            double efficiency = (speedup / num_threads) * 100.0;
            
            printf("%-12s | %12.6f | %10.2f | %9.2f%%\n", 
                   schedule_type, time, speedup, efficiency);
        }
        printf("\n");
    }
    
    free_matrix(A, N);
    free_matrix(B, N);
    free_matrix(C, N);
    
    return 0;
}
