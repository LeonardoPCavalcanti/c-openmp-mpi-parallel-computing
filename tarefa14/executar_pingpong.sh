#!/bin/bash
#SBATCH --job-name=mpi_pingpong
#SBATCH --partition=amd-512
#SBATCH --time=0-0:15       # 15 minutos é suficiente
#SBATCH --nodes=2           # Pede 2 nós diferentes
#SBATCH --ntasks-per-node=1   # Garante 1 processo por nó

EXECUTAVEL="./pingpong"
OUTPUT_FILE="resultados.csv"

echo "Compilando o programa..."
mpicc pingpong.c -o $EXECUTAVEL

# Adiciona o cabeçalho ao arquivo de resultados
echo "tamanho_bytes,tempo_ms" > $OUTPUT_FILE

# Testa tamanhos de mensagem em potências de 2, de 8 bytes (2^3) a 64 MB (2^26)
echo "Iniciando testes..."
for power in {3..26}; do
    SIZE=$((2**power))
    echo "Testando com tamanho de mensagem: $SIZE bytes"
    srun $EXECUTAVEL $SIZE >> $OUTPUT_FILE
done

echo "Testes concluídos. Resultados salvos em $OUTPUT_FILE"