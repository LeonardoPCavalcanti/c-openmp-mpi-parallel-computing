#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 1000000LL

int main(void){
    long long dentro = 0;
    srand(12345);

    double t0 = omp_get_wtime();
    double x, y;
    #pragma omp parallel for private(x,y)
    for(long long i = 0; i < N; ++i){
        x = (double)rand()/RAND_MAX;
        y = (double)rand()/RAND_MAX;
        if (x*x + y*y <= 1.0) dentro++; // ainda race
    }
    double t1 = omp_get_wtime();

    double pi = 4.0*(double)dentro/(double)N;
    printf("[private] N=%lld | pi=%.6f | tempo=%.6f s  (INCORRETO)\n", N, pi, t1-t0);
    return 0;
}
