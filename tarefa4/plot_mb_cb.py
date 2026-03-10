# plot_final_tarefa4.py
import matplotlib.pyplot as plt

# Número de threads
threads = [1, 2, 3, 4, 5, 6, 7, 8]

# --- SEUS DADOS FINAIS ---
# Memory-bound (matrix col-major)
tempos_mem = [9.610834, 4.469845, 3.108002, 2.895323,
              2.457902, 2.323605, 2.094527, 1.885860]

# CPU-bound (cálculos matemáticos intensivos)
tempos_cpu = [26.342810, 13.299536, 8.834155, 6.680801, 
              5.387896, 4.753835, 3.915503, 3.672929]

plt.figure(figsize=(9, 5))

# Plot Memory-Bound
plt.plot(threads, tempos_mem, marker='s', markersize=7,
         linewidth=2, linestyle='-', color='blue', label='Memory-Bound')

# Plot CPU-Bound
plt.plot(threads, tempos_cpu, marker='^', markersize=7,
         linewidth=2, linestyle='-', color='red', label='CPU-Bound')

# Títulos e legendas
plt.title('Comparativo de Desempenho: Memory-Bound vs. CPU-Bound')
plt.xlabel('Número de Threads')
plt.ylabel('Tempo de Execução (s)')
plt.xticks(threads)
plt.grid(True, linestyle='--', alpha=0.4)
plt.legend(loc='upper right', frameon=True)
plt.tight_layout()

# Salva e mostra
plt.savefig('grafico_comparativo_tarefa4.png', dpi=200)
plt.show()
