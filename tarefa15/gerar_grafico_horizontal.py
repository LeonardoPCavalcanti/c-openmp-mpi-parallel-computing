import matplotlib.pyplot as plt
import numpy as np
import re
import pandas as pd

# Usa um estilo visual diferente para o gráfico
plt.style.use('seaborn-v0_8-whitegrid')

# --- Paleta de Cores (VOCÊ PODE ALTERAR AQUI) ---
# Define as cores para as barras: [Bloqueante, Wait, Test]
# Você pode usar nomes de cores ("red", "blue") ou códigos hexadecimais.
CORES = ['#999999', '#348ABD', '#5D9CEC'] # Cinza, Azul Escuro, Azul Claro

def ler_tempos(nome_arquivo):
    """Lê um arquivo de texto e extrai TODOS os tempos de execução."""
    tempos = []
    try:
        with open(nome_arquivo, 'r') as f:
            for linha in f:
                match = re.search(r'(\d+\.\d+)', linha)
                if match:
                    tempos.append(float(match.group(1)))
    except FileNotFoundError:
        print(f"Aviso: Arquivo '{nome_arquivo}' não encontrado.")
    return tempos

# --- 1. Carregar e Estruturar os Dados ---
processos = [8, 16, 32, 64]
tempos_bloqueante = ler_tempos('tempo_bloqueante.txt')
tempos_wait = ler_tempos('tempo_wait.txt')
tempos_test = ler_tempos('tempo_test.txt')

if not (len(tempos_bloqueante) == len(processos)):
    print("Erro: A quantidade de resultados não corresponde à quantidade de testes.")
    exit()

df = pd.DataFrame({
    'Processos': processos,
    'Bloqueante': tempos_bloqueante,
    'Wait': tempos_wait,
    'Test': tempos_test
})

# --- 2. Gerar um Gráfico para Cada Teste ---
print("Gerando gráficos individuais com a nova paleta de cores...")

for index, row in df.iterrows():
    num_processos = int(row['Processos'])
    
    estrategias = ['Bloqueante', 'Não Bloqueante (Wait)', 'Não Bloqueante (Test)']
    tempos = [row['Bloqueante'], row['Wait'], row['Test']]
    
    tempo_baseline = tempos[0]
    speedups = [tempo_baseline / t for t in tempos]
    
    # --- Cria uma nova figura para cada gráfico ---
    fig, ax = plt.subplots(figsize=(12, 6))
    
    # Usa a nova paleta de cores definida no topo do script
    bars = ax.barh(estrategias, tempos, color=CORES, height=0.6)
    
    ax.invert_yaxis()
    
    # --- Adiciona Detalhes e Legendas ---
    ax.set_xlabel('Tempo de Execução (s)', fontsize=12)
    ax.set_title(f'Comparação de Desempenho para {num_processos} Processos', fontsize=16)
    
    ax.xaxis.grid(True, linestyle='--', which='major', color='grey', alpha=.7)
    ax.yaxis.grid(False)
    
    # Adiciona os valores de tempo e speedup nas barras
    for i, bar in enumerate(bars):
        width = bar.get_width()
        ax.text(width * 1.01, bar.get_y() + bar.get_height()/2, f'{width:.4f} s', 
                va='center', ha='left', fontsize=10)
        ax.text(0.001, bar.get_y() + bar.get_height()/2, f'{speedups[i]:.2f}x', 
                va='center', ha='left', fontsize=10, color='white', weight='bold')

    fig.tight_layout()

    # --- Salva o gráfico com um nome de arquivo único ---
    nome_arquivo_grafico = f'grafico_horizontal_{num_processos}_processos.png'
    plt.savefig(nome_arquivo_grafico)
    print(f" -> Gráfico salvo como '{nome_arquivo_grafico}'")
    
    plt.close(fig)

print("\n✅ Processo concluído com sucesso!")