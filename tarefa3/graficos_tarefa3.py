# graficos_tarefa3.py
# Lê pi_results.csv (formato: Metodo,Iteracoes,PiCalculado,ErroAbsoluto,Tempo_s)
# e gera dois gráficos:
# 1) tempo vs iterações (sequencial x ILP), log-log
# 2) trade-off (erro e tempo vs iterações) para um método escolhido (ILP por padrão)

import csv
import math
import matplotlib.pyplot as plt

csv_path = "pi_results.csv"

# Carrega os dados
seq = {"it": [], "erro": [], "tempo": []}
ilp = {"it": [], "erro": [], "tempo": []}

with open(csv_path, newline="") as f:
    r = csv.DictReader(f)
    for row in r:
        metodo = row["Metodo"].strip()
        it = int(row["Iteracoes"])
        erro = float(row["ErroAbsoluto"])
        tempo = float(row["Tempo_s"])
        if metodo.lower().startswith("seq"):
            seq["it"].append(it); seq["erro"].append(erro); seq["tempo"].append(tempo)
        else:
            ilp["it"].append(it); ilp["erro"].append(erro); ilp["tempo"].append(tempo)

# Ordena por número de iterações (só por garantia)
def sort_by_iterations(d):
    order = sorted(range(len(d["it"])), key=lambda k: d["it"][k])
    for k in d:
        d[k] = [d[k][i] for i in order]

sort_by_iterations(seq)
sort_by_iterations(ilp)

# -------------------------------------------------------------------
# Gráfico 1: Tempo vs Iterações (Sequencial x ILP) — log-log
# -------------------------------------------------------------------
plt.figure(figsize=(8.5, 3.2), dpi=160)

plt.loglog(seq["it"], seq["tempo"], marker="o", linestyle="-", linewidth=1.5,
           markersize=4, label="Sequencial (com dependência)")
plt.loglog(ilp["it"], ilp["tempo"], marker="s", linestyle="--", linewidth=1.5,
           markersize=4, label="ILP (4 acumuladores)")

plt.title("Tempo de Execução vs. Número de Iterações (Log-Log)")
plt.xlabel("Número de Iterações")
plt.ylabel("Tempo (segundos)")
plt.grid(True, which="both", ls=":", alpha=0.4)
plt.legend(loc="upper left")
plt.tight_layout()
plt.savefig("graf_tempo_seq_ilp.png")
plt.close()

# -------------------------------------------------------------------
# Gráfico 2: Trade-off Precisão x Tempo vs Iterações (eixo duplo)
#           -> por padrão, usa ILP. mude method = "seq" para sequencial
# -------------------------------------------------------------------
method = "ilp"   # "ilp" ou "seq"
D = ilp if method == "ilp" else seq
metodo_label = "ILP (4 acumuladores)" if method == "ilp" else "Sequencial"

fig, ax1 = plt.subplots(figsize=(9.5, 5.2), dpi=140)

# Erro absoluto (eixo esquerdo, log)
ax1.set_xscale("log")
ax1.set_yscale("log")
ax1.plot(D["it"], D["erro"], marker="o", linestyle="-", linewidth=2,
         label="Erro Absoluto", color="#c23b22")
ax1.set_xlabel("Número de Iterações (escala log)")
ax1.set_ylabel("Erro Absoluto", color="#c23b22")
ax1.tick_params(axis='y', labelcolor="#c23b22")

# Tempo (eixo direito, log)
ax2 = ax1.twinx()
ax2.set_yscale("log")
ax2.plot(D["it"], D["tempo"], marker="x", linestyle="--", linewidth=2,
         label="Tempo Decorrido", color="#1f77b4")
ax2.set_ylabel("Tempo Decorrido (segundos)", color="#1f77b4")
ax2.tick_params(axis='y', labelcolor="#1f77b4")

# Título e grades
plt.title(f"Análise de Precisão e Tempo vs. Iterações — {metodo_label}")
ax1.grid(True, which="both", ls=":", alpha=0.35)

# Legenda combinada
h1, l1 = ax1.get_legend_handles_labels()
h2, l2 = ax2.get_legend_handles_labels()
ax1.legend(h1 + h2, l1 + l2, loc="upper center")

fig.tight_layout()
plt.savefig("graf_tradeoff_ilp.png")  # se trocar method para "seq", renomeie se quiser
plt.close()

print("OK! Gerados: graf_tempo_seq_ilp.png e graf_tradeoff_ilp.png")
