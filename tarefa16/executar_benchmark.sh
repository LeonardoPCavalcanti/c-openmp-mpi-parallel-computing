#!/bin/bash
#SBATCH --job-name=benchmark_matriz_vetor
#SBATCH --partition=amd-512
#SBATCH --time=0-1:00
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=32

# ======================================================================
# CONFIGURAÇÕES
# ======================================================================
EXECUTAVEL="./matriz_vetor"
OUTPUT_FILE="benchmark_resultados.csv"
NUM_PROCESSOS_ARRAY=(2 4 8 16 32 64)
TAMANHOS_DE_MATRIZ=(1024 2048 4096 8192 16384 32768)

# ======================================================================
# FUNÇÃO PARA EXECUTAR E CAPTURAR O TEMPO
# ======================================================================
function executar_benchmark {
    local TAMANHO=$1
    local N_PROCS=$2

    # Executa com 'srun' e captura a saída que contém "TEMPO:"
    TEMPO_SAIDA=$(srun -n "$N_PROCS" "$EXECUTAVEL" "$TAMANHO" 2>&1 | \
                  grep "TEMPO:" | \
                  sed 's/TEMPO://') # Remove o rótulo "TEMPO:"
    
    # Verifica se a captura do tempo foi bem-sucedida
    if [[ "$TEMPO_SAIDA" =~ ^[0-9].*\.[0-9]*$ ]]; then
        echo "${TAMANHO},${N_PROCS},${TEMPO_SAIDA}" >> "$OUTPUT_FILE"
        echo "  [OK] Matriz ${TAMANHO}x${TAMANHO}, P=${N_PROCS} | Tempo: ${TEMPO_SAIDA}s"
    else
        echo "  [FALHA] Matriz ${TAMANHO}x${TAMANHO}, P=${N_PROCS} | Erro na captura do tempo. Saída: '$TEMPO_SAIDA'"
    fi
}

# ======================================================================
# FLUXO PRINCIPAL
# ======================================================================

echo "Compilando o código C..."
mpicc matriz_vetor.c -o "$EXECUTAVEL"
if [ $? -ne 0 ]; then
    echo "ERRO: Falha na compilação."
    exit 1
fi

echo "Iniciando o Benchmark MPI. Resultados serão salvos em $OUTPUT_FILE"
echo "Tamanho_Matriz,Num_Processos,Tempo_s" > "$OUTPUT_FILE"

for M in "${TAMANHOS_DE_MATRIZ[@]}"; do
    echo -e "\n--- Testando Matriz ${M}x${M} ---"
    for NP in "${NUM_PROCESSOS_ARRAY[@]}"; do
        if [ "$NP" -le "$M" ]; then
            executar_benchmark "$M" "$NP"
        fi
    done
done

echo -e "\nBenchmark concluído."