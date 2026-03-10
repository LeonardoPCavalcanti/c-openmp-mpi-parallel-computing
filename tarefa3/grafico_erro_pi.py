#!/usr/bin/env python3
# Lê pi_results.csv e plota Erro Absoluto x Iteracoes (Sequencial vs ILP)

import csv
import math
import matplotlib.pyplot as plt

arquivo = "pi_results.csv"

# Coletar dados
series = {"Sequencial": {"iters": [], "erro": []},
          "ILP":        {"iters": [], "erro": []}}

with open(arquivo, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        metodo = row["Metodo"].strip()
        it = int(row["Iteracoes"])
        err = float(row["ErroAbsoluto"])
        if metodo in series:
            series[metodo]["iters"].append(it)
            series[metodo]["erro"].append(err)

# Ordenar por número de iterações (caso a CSV esteja fora de ordem)
for m in series:
    pares = sorted(zip(series[m]["iters"], series[m]["erro"]))
    series[m]["iters"] = [p[0] for p in pares]
    series[m]["erro"]  = [p[1] for p in pares]

# Plot: usar escala log para evidenciar a queda do erro
plt.figure(figsize=(7,4.5))
for m, sty in series.items():
    plt.plot(sty["iters"], sty["erro"], marker="o", label=m)

plt.xscale("log")
plt.yscale("log")
plt.xlabel("Iterações (escala log)")
plt.ylabel("Erro absoluto |π_calc − π| (escala log)")
plt.title("Diminuição do erro absoluto na aproximação de π")
plt.grid(True, which="both", linestyle="--", alpha=0.4)
plt.legend()
plt.tight_layout()
plt.savefig("grafico_erro_pi.png", dpi=150)
plt.show()
