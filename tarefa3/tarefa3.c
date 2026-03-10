#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// --- Versão Sequencial ---
double calcular_pi_sequencial(long long num_iteracoes) {
    double soma = 0.0;
    int sinal = 1;  // alterna entre + e -
    for (long long k = 0; k < num_iteracoes; k++) {
        double termo = (double)sinal / (2.0 * k + 1.0);
        soma += termo;
        sinal *= -1;
    }
    return 4.0 * soma;
}

// --- Versão com ILP (4 acumuladores) ---
double calcular_pi_ilp(long long num_iteracoes) {
    double acumulador1 = 0.0, acumulador2 = 0.0;
    double acumulador3 = 0.0, acumulador4 = 0.0;

    for (long long k = 0; k < num_iteracoes; k += 4) {
        acumulador1 += 1.0 / (2.0 * (k + 0) + 1.0);
        acumulador2 -= 1.0 / (2.0 * (k + 1) + 1.0);
        acumulador3 += 1.0 / (2.0 * (k + 2) + 1.0);
        acumulador4 -= 1.0 / (2.0 * (k + 3) + 1.0);
    }

    return 4.0 * (acumulador1 + acumulador2 + acumulador3 + acumulador4);
}

int main() {
    long long lista_iteracoes[] = {
        10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000
    };
    int total_testes = sizeof(lista_iteracoes) / sizeof(lista_iteracoes[0]);

    FILE *arquivo_csv = fopen("pi_results.csv", "w");
    fprintf(arquivo_csv, "Metodo,Iteracoes,PiCalculado,ErroAbsoluto,Tempo_s\n");

    for (int i = 0; i < total_testes; i++) {
        long long num_iteracoes = lista_iteracoes[i];
        struct timespec inicio, fim;

        // --- Sequencial ---
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        double pi_seq = calcular_pi_sequencial(num_iteracoes);
        clock_gettime(CLOCK_MONOTONIC, &fim);
        double tempo_seq = (fim.tv_sec - inicio.tv_sec) +
                           (fim.tv_nsec - inicio.tv_nsec) / 1e9;
        double erro_seq = fabs(M_PI - pi_seq);

        fprintf(arquivo_csv, "Sequencial,%lld,%.15f,%.15f,%.9f\n",
                num_iteracoes, pi_seq, erro_seq, tempo_seq);

        // --- ILP ---
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        double pi_ilp = calcular_pi_ilp(num_iteracoes);
        clock_gettime(CLOCK_MONOTONIC, &fim);
        double tempo_ilp = (fim.tv_sec - inicio.tv_sec) +
                           (fim.tv_nsec - inicio.tv_nsec) / 1e9;
        double erro_ilp = fabs(M_PI - pi_ilp);

        fprintf(arquivo_csv, "ILP,%lld,%.15f,%.15f,%.9f\n",
                num_iteracoes, pi_ilp, erro_ilp, tempo_ilp);
    }

    fclose(arquivo_csv);
    printf("Resultados salvos em pi_results.csv\n");
    return 0;
}
