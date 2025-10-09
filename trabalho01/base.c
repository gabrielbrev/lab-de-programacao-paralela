#include <stdio.h>
#include <time.h>

int main () {
    int x, soma = 0, subtracao = 0, TAM = 1200;
    long long int mult = 1;
    int vet[TAM];
    clock_t inicio, fim;
    double tempo_execucao;
    
    inicio = clock();
    
    for (x = 0; x < TAM; x++) {
        vet[x] = x + 1;
    }

    for (x = 0; x < TAM; x++) {
        // printf("vet[%d] = %d\n", x, vet[x]);    
    }
    
    for (x = 0; x < TAM; x++) {
        soma = soma + vet[x];
        subtracao = subtracao - vet[x];
        mult = mult * vet[x];
    }
    
    fim = clock();
    tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    
    printf("Soma = %d\n", soma);
    printf("Subtracao = %d\n", subtracao);
    printf("Multiplicacao = %lld\n", mult);
    printf("Tempo de execucao: %f segundos\n", tempo_execucao);
}
