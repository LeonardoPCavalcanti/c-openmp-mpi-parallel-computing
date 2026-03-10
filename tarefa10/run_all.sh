set -euo pipefail
N=${1:-100000000}

binaries=(pi_critical_comp pi_atomic_comp pi_critical_priv pi_atomic_priv pi_reduction)
for b in "${binaries[@]}"; do
  if [[ ! -x "./$b" ]]; then
    echo "Binário ./$b não encontrado. Compile com: make"
    exit 1
  fi
done

for th in 1 2 4 8; do
  echo "=============================="
  echo "OMP_NUM_THREADS=$th | N=$N"
  echo "=============================="
  export OMP_NUM_THREADS=$th
  for b in "${binaries[@]}"; do
    ./"$b" "$N"
  done
  echo
done
