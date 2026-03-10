// primes_omp_ok.c — paralelo correto sem reduction/critical (usa apenas parallel for)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define LIMITE_PADRAO 300000u
#define MAX_THREADS   8

static double agora(void){
    struct timeval tv; gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec/1e6;
}

static int eh_primo(unsigned int x){
    if (x < 2) return 0;
    if (x == 2) return 1;
    if (x % 2 == 0) return 0;
    unsigned int r = (unsigned int)floor(sqrt((double)x));
    for (unsigned int d = 3; d <= r; d += 2)
        if (x % d == 0) return 0;
    return 1;
}

int main(void){
    unsigned int limite = LIMITE_PADRAO;

    printf("Contando primos em [2, %u] (paralelo correto, sem reduction)\n\n", limite);

    for (int thr = 1; thr <= MAX_THREADS; ++thr) {
        omp_set_num_threads(thr);

        // baldes por thread: cada thread escreve só no seu índice
        unsigned long local[MAX_THREADS] = {0};

        double t0 = agora();
        #pragma omp parallel for
        for (unsigned int k = 2; k <= limite; ++k) {
            int tid = omp_get_thread_num();      // 0..thr-1
            if (eh_primo(k)) local[tid] += 1;    // sem disputa entre threads
        }
        double t1 = agora();

        // soma final em série (sem diretiva extra)
        unsigned long total = 0;
        for (int t = 0; t < thr; ++t) total += local[t];

        printf("[OMP ok] threads=%d | primos=%lu | tempo=%.6f s\n",
               thr, total, t1 - t0);
    }
    return 0;
}
