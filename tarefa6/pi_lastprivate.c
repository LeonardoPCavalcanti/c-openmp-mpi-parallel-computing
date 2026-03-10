#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 1000000LL

int main(void){
    long long dentro = 0;
    long long local = 0;
    srand(12345);

    double t0 = omp_get_wtime();
    #pragma omp parallel shared(dentro)
    {
        #pragma omp for lastprivate(local)
        for(long long i = 0; i < N; ++i){
            double x = (double)rand()/RAND_MAX;
            double y = (double)rand()/RAND_MAX;
            if (x*x + y*y <= 1.0) local++;
        }
    }
    // apenas a última cópia de local sobrevive
    dentro = local;

    double t1 = omp_get_wtime();
    double pi = 4.0*(double)dentro/(double)N;
    printf("[lastprivate] N=%lld | pi=%.6f | tempo=%.6f s  (INCORRETO — última cópia)\n",
           N, pi, t1-t0);
    return 0;
}
