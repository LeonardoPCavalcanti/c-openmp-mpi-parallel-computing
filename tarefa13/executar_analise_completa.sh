#!/bin/bash
#SBATCH --job-name=analise_completa_ns
#SBATCH --time=0-12:00  # Aumentado para 12 horas devido ao volume de testes
#SBATCH --partition=amd-512
#SBATCH --cpus-per-task=64

# --- Parâmetros da Análise de Escalabilidade ---

# Lista de número de threads (núcleos) a serem testados pelo pascalanalyzer
LISTA_CORES="1,2,4,8,16,32,64"

# Lista de tamanhos de grade (input) a serem testados (versão completa)
LISTA_INPUTS="32,64,128,512,1024,2048,4096"

# Nome do seu programa executável (compilado)
EXECUTAVEL="./ns_otimizado"

# --- Início dos Testes ---
echo "Iniciando análise completa de escalabilidade e afinidade."
echo "Executável: $EXECUTAVEL"
echo "Lista de Cores: $LISTA_CORES"
echo "Lista de Inputs: $LISTA_INPUTS"
echo "========================================================"

# Teste 1: Política 'spread'
echo "INICIANDO TESTE COM OMP_PROC_BIND=spread"
export OMP_PROC_BIND=spread
pascalanalyzer $EXECUTAVEL -c $LISTA_CORES -i $LISTA_INPUTS --inst aut -o resultado_spread.json
echo "Análise 'resultado_spread.json' gerada."
echo "========================================================"

# Teste 2: Política 'close'
echo "INICIANDO TESTE COM OMP_PROC_BIND=close"
export OMP_PROC_BIND=close
pascalanalyzer $EXECUTAVEL -c $LISTA_CORES -i $LISTA_INPUTS --inst aut -o resultado_close.json
echo "Análise 'resultado_close.json' gerada."
echo "========================================================"

# Teste 3: Política 'false' (sem afinidade)
echo "INICIANDO TESTE COM OMP_PROC_BIND=false"
export OMP_PROC_BIND=false
pascalanalyzer $EXECUTAVEL -c $LISTA_CORES -i $LISTA_INPUTS --inst aut -o resultado_false.json
echo "Análise 'resultado_false.json' gerada."
echo "========================================================"

# Teste 4: Política 'true' (afinidade padrão do sistema)
echo "INICIANDO TESTE COM OMP_PROC_BIND=true"
export OMP_PROC_BIND=true
pascalanalyzer $EXECUTAVEL -c $LISTA_CORES -i $LISTA_INPUTS --inst aut -o resultado_true.json
echo "Análise 'resultado_true.json' gerada."
echo "========================================================"

# Teste 5: Política 'master'
echo "INICIANDO TESTE COM OMP_PROC_BIND=master"
echo "AVISO: Este teste será EXTREMAMENTE LENTO com muitos núcleos."
export OMP_PROC_BIND=master
pascalanalyzer $EXECUTAVEL -c $LISTA_CORES -i $LISTA_INPUTS --inst aut -o resultado_master.json
echo "Análise 'resultado_master.json' gerada."
echo "========================================================"

echo "TODOS OS TESTES FORAM CONCLUÍDOS."