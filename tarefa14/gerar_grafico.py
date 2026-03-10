import pandas as pd
import matplotlib.pyplot as plt
import io

# Dados de latência (tamanho em bytes, tempo em ms)
# O formato é simples: "tamanho,tempo"
data = """
8,0.114373250
16,0.134687195
32,0.128130880
64,0.096475660
128,0.118009100
256,0.133662510
512,0.114385420
1024,0.129886240
2048,0.170521035
4096,0.129231110
8192,0.128886055
16384,0.120026105
32768,0.134349405
65536,0.146521550
131072,0.145847980
262144,0.189618490
524288,0.159228300
1048576,0.222957535
2097152,0.302293740
4194304,0.476981025
8388608,0.827874370
16777216,1.496058730
33554432,2.869822975
67108864,5.618823095

"""

# Usar o pandas para ler os dados diretamente da string
# io.StringIO trata a string como se fosse um arquivo
df = pd.read_csv(io.StringIO(data), header=None, names=['tamanho_bytes', 'tempo_ms'])

# Opcional: Imprimir o início do DataFrame para verificar se foi lido corretamente
print("Dados carregados com sucesso:")
print(df.head())

# --- Código de plotagem ---

# Criar o gráfico de linha
plt.figure(figsize=(10, 6))
plt.plot(df['tamanho_bytes'], df['tempo_ms'], marker='o', linestyle='-')
plt.xscale('log', base=2) # Usar escala logarítmica na base 2 para o eixo X
plt.grid(True, which="both", ls="--", color='0.65')

# Adicionar títulos e rótulos
plt.title('Latência de Comunicação vs. Tamanho da Mensagem', fontsize=16)
plt.xlabel('Tamanho da Mensagem (bytes) [Escala Logarítmica - base 2]', fontsize=12)
plt.ylabel('Tempo Médio de Troca (ms)', fontsize=12)

# Melhorar os marcadores do eixo X para mostrar tamanhos comuns (KB, MB)
ticks = [2**i for i in range(3, 27) if 2**i <= df['tamanho_bytes'].max()]
labels = [f'{t // 1024} KB' if t >= 1024 and t < 1024**2 else f'{t // 1024**2} MB' if t >= 1024**2 else f'{t} B' for t in ticks]
plt.xticks(ticks, labels, rotation=45, ha="right")


# Salvar o gráfico
plt.tight_layout()
plt.savefig('latencia_vs_tamanho.png')

print("\nGráfico 'latencia_vs_tamanho.png' foi salvo com sucesso.")