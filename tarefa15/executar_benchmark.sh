#!/bin/bash
#SBATCH --job-name=calor_1d_benchmark_full
#SBATCH --partition=amd-512
#SBATCH --time=0-0:20       # 20 minutos de tempo de execução
#SBATCH --nodes=2           # Aloca 2 nós para garantir comunicação de rede
#SBATCH --ntasks=64         # Total de processos para o maior teste
#SBATCH --ntasks-per-node=32  # Distribui 32 processos por nó

# ===============================================
# CONFIGURAÇÕES
# ===============================================

# Lista de número de processos MPI para testar
LISTA_DE_PROCESSOS="8 16 32 64"

# Nomes dos arquivos C (baseado nos arquivos que você enviou)
V1_C="bloqueante.c"
V2_C="n_bloqueante_wait.c"
V3_C="n_bloqueante_test.c"

# Nomes dos executáveis que serão gerados
V1_EXE="bloqueante"
V2_EXE="n_bloqueante_wait"
V3_EXE="n_bloqueante_test"

# Nomes dos arquivos de saída para os tempos
V1_OUT="tempo_bloqueante.txt"
V2_OUT="tempo_wait.txt"
V3_OUT="tempo_test.txt"

# ===============================================
# COMPILAÇÃO E PREPARAÇÃO
# ===============================================
echo "--- 🛠️ Compilando as três versões do código ---"
mpicc ${V1_C} -o ${V1_EXE}
mpicc ${V2_C} -o ${V2_EXE}
mpicc ${V3_C} -o ${V3_EXE}
echo "Compilação concluída."
echo ""

# Limpa os arquivos de resultado de execuções antigas para começar do zero
rm -f ${V1_OUT} ${V2_OUT} ${V3_OUT}

# ===============================================
# EXECUÇÃO DO BENCHMARK
# ===============================================

echo "--- 🚀 INICIANDO BENCHMARK COMPLETO ---"
for NP in ${LISTA_DE_PROCESSOS}; do
    echo ""
    echo "--- Executando com ${NP} processos ---"

    echo "  -> Versão Bloqueante..."
    mpirun -np ${NP} ./${V1_EXE} >> ${V1_OUT}

    echo "  -> Versão Não-Bloqueante (Wait)..."
    mpirun -np ${NP} ./${V2_EXE} >> ${V2_OUT}

    echo "  -> Versão Não-Bloqueante (Test)..."
    mpirun -np ${NP} ./${V3_EXE} >> ${V3_OUT}
done

echo "----------------------------------------------------"
echo "✅ Benchmark concluído. Tempos salvos nos arquivos:"
echo "   - ${V1_OUT}"
echo "   - ${V2_OUT}"
echo "   - ${V3_OUT}"
echo ""
echo "Próximo passo: execute o script Python para gerar os gráficos."