# plot_6steps.py
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

files = [
    "snap_t00000.csv",
    "snap_t02000.csv",
    "snap_t04000.csv",
    "snap_t06000.csv",
    "snap_t08000.csv",
    "snap_t10000.csv"
]

# Carrega todos os snapshots
Zs = [np.loadtxt(f, delimiter=',') for f in files]
nx, ny = Zs[0].shape
X, Y = np.meshgrid(np.arange(nx), np.arange(ny), indexing='ij')

# Escala fixa para todos os gráficos (garante comparação)
zmin = min(Z.min() for Z in Zs)
zmax = max(Z.max() for Z in Zs)
# Se preferir travar manualmente (por exemplo entre 1 e 3):
# zmin, zmax = 1.0, 3.0

# Cria a figura com 6 subplots (2 linhas × 3 colunas)
fig = plt.figure(figsize=(14,6))

for k, Z in enumerate(Zs, 1):
    ax = fig.add_subplot(2, 3, k, projection='3d')
    surf = ax.plot_surface(X, Y, Z, cmap='viridis', linewidth=0, antialiased=True)
    step = files[k-1].split('_t')[1].split('.')[0]
    ax.set_title(f"Step {step}")
    ax.set_zlim(zmin, zmax)

# Título global
fig.suptitle("Evolução do Campo de Velocidade U (Superfície 3D)", fontsize=14)

# Ajusta espaço para abrir área da colorbar à direita
fig.subplots_adjust(right=0.87)

# Adiciona a barra de cores global do lado direito
cbar_ax = fig.add_axes([0.90, 0.15, 0.02, 0.7])  # [x, y, largura, altura]
mappable = plt.cm.ScalarMappable(cmap='viridis')
mappable.set_array(np.array([zmin, zmax]))
cb = fig.colorbar(mappable, cax=cbar_ax)
cb.set_label("Velocidade U")

# Salva em alta resolução
plt.savefig("evolucao_6steps.png", dpi=220)
plt.show()
