#!/usr/bin/env bash
set -euo pipefail
NTHREADS_LIST="${1:-"1 2 4 8"}"

bins=(ns_omp_simple ns_omp_opt_static ns_omp_opt_dynamic ns_omp_opt_guided)

for th in $NTHREADS_LIST; do
  export OMP_NUM_THREADS=$th
  echo "================ OMP_NUM_THREADS=$th ================"
  for b in "${bins[@]}"; do
    if [[ -x "./$b" ]]; then
      ./"$b" 1
    fi
  done
done
