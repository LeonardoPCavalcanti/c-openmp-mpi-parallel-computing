#!/usr/bin/env bash
set -euo pipefail

# 1) Campo uniforme (perturb=0): deve permanecer estável
./ns_serial 0
./ns_omp_simple 0
./ns_omp_opt_static 0

# 2) Com perturbação (perturb=1): difunde (L2 cai e max|v| diminui ao longo do tempo)
./ns_serial 1
./ns_omp_simple 1
./ns_omp_opt_static 1
