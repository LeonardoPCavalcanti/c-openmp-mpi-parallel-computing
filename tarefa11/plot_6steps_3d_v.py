import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

files = [
    "snap_v_t00000.csv",
    "snap_v_t02000.csv",
    "snap_v_t04000.csv",
    "snap_v_t06000.csv",
    "snap_v_t08000.csv",
    "snap_v_t10000.csv"
]

Zs = [np.loadtxt(f, delimiter=',') for f in files]
nx, ny = Zs[0].shape
X, Y = np.meshgrid(np.arange(nx), np.arange(ny), indexing='ij')

# escala fixa para todos
zmin = min(Z.min() for Z in Zs)
zmax = max(Z.max() for Z in Zs)
# zmin, zmax = 1.0, 3.0  # opcional: travar manualmente

fig = plt.figure(figsize=(14,6))
for k, Z in enumerate(Zs, 1):
    ax = fig.add_subplot(2, 3, k, projection='3d')
    ax.plot_surface(X, Y, Z, cmap='viridis', linewidth=0, antialiased=True)
    step = files[k-1].split('_t')[1].split('.')[0]
    ax.set_title(f"Step {step}")
    ax.set_zlim(zmin, zmax)

fig.suptitle("Evolução do Campo de Velocidade V (Superfície 3D)", fontsize=14)
fig.subplots_adjust(right=0.87)
cbar_ax = fig.add_axes([0.90, 0.15, 0.02, 0.7])
mappable = plt.cm.ScalarMappable(cmap='viridis')
mappable.set_array(np.array([zmin, zmax]))
cb = fig.colorbar(mappable, cax=cbar_ax)
cb.set_label("Velocidade V")

plt.savefig("evolucao_6steps_V_3d.png", dpi=220)
plt.show()
