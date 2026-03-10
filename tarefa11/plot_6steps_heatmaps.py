import numpy as np
import matplotlib.pyplot as plt

def load_series(prefix):
    files = [f"{prefix}_t00000.csv", f"{prefix}_t02000.csv", f"{prefix}_t04000.csv",
             f"{prefix}_t06000.csv", f"{prefix}_t08000.csv", f"{prefix}_t10000.csv"]
    Zs = [np.loadtxt(f, delimiter=',') for f in files]
    return files, Zs

def plot_grid(files, Zs, title, outpng):
    # escala fixa global
    vmin = min(Z.min() for Z in Zs)
    vmax = max(Z.max() for Z in Zs)

    fig, axes = plt.subplots(2, 3, figsize=(14,6))
    axes = axes.ravel()

    for k, (ax, Z) in enumerate(zip(axes, Zs)):
        im = ax.imshow(Z.T, origin='lower', aspect='auto', cmap='viridis',
                       vmin=vmin, vmax=vmax)
        step = files[k].split('_t')[1].split('.')[0]
        ax.set_title(f"Step {step}")
        ax.set_xlabel('i (x)'); ax.set_ylabel('j (y)')

    fig.suptitle(title, fontsize=14)
    fig.subplots_adjust(right=0.87)
    cbar_ax = fig.add_axes([0.90, 0.15, 0.02, 0.7])
    cb = fig.colorbar(im, cax=cbar_ax)
    cb.set_label('Amplitude')

    plt.savefig(outpng, dpi=220)
    plt.show()

if __name__ == "__main__":
    files_u, Zs_u = load_series("snap_u")
    plot_grid(files_u, Zs_u, "Evolução de U (Heatmaps 2D)", "evolucao_6steps_U_2d.png")

    files_v, Zs_v = load_series("snap_v")
    plot_grid(files_v, Zs_v, "Evolução de V (Heatmaps 2D)", "evolucao_6steps_V_2d.png")
