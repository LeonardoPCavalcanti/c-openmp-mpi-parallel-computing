#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 20000000LL
#define MAX_THREADS 8

int main(void) {
    printf("CPU-BOUND | N=%lld\n", N);

    for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads++) {
        omp_set_num_threads(num_threads);

        double soma_thread[MAX_THREADS] = {0};
        double inicio = omp_get_wtime();

        #pragma omp parallel for
        for (long long i = 0; i < N; i++) {
            int tid = omp_get_thread_num();

            double x = (i & 1023) + 1.0;
            double y = ((i >> 1) & 1023) + 2.0;

            for (int k = 0; k < 100; k++) {
                x = x*1.000001 + y*0.999999;
                y = y*0.999997 + x*1.000002;
                double temp = x - 0.333333333 * y;
                y = y + 0.714285714 * x - 0.25 * temp;
                x = sqrt(fabs(x) + 1.0);
                y = sqrt(fabs(y) + 1.0);
            }
            soma_thread[tid] += x + y;
        }

        double fim = omp_get_wtime();

        double soma_total = 0.0;
        for (int t = 0; t < num_threads; t++) soma_total += soma_thread[t];

        printf("threads=%d | tempo=%.6f s | soma=%.3f\n",
               num_threads, fim - inicio, soma_total);
    }
    return 0;
}
