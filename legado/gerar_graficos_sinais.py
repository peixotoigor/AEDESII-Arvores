import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from pathlib import Path

OUTPUT_DIR = Path('figuras')
OUTPUT_DIR.mkdir(exist_ok=True)

signal = np.array([0, 1, 3, 2, -1, 2, 1, 0], dtype=float)
N = len(signal)
indices = np.arange(N)

def refletir(sinal):
    return sinal[::-1].copy()

def escalar(sinal, fator):
    return sinal[::fator].copy()

def alterar_amplitude(sinal, fator):
    return fator * sinal

def deslocar(sinal, k):
    resultado = np.zeros_like(sinal)
    for i in range(len(sinal)):
        if 0 <= i - k < len(sinal):
            resultado[i] = sinal[i - k]
    return resultado

def deslocar_com_mudanca_dominio(sinal, k):
    indices_deslocados = np.arange(len(sinal)) + k
    return indices_deslocados, sinal.copy()

def degrau(n, amplitude):
    return np.full(n, amplitude, dtype=float)

def somar(a, b):
    return a + b

factor_dec = 2
factor_amp = 2.0
shift_k = 2
step_amp = 1.5

reflected = refletir(signal)
decimated = escalar(signal, factor_dec)
amplified = alterar_amplitude(signal, factor_amp)
shifted = deslocar(signal, shift_k)
shifted_domain_indices, shifted_domain_signal = deslocar_com_mudanca_dominio(signal, shift_k)
step_signal = degrau(N, step_amp)
summed = somar(signal, step_signal)

plt.rcParams.update({
    'font.size': 11,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'figure.titlesize': 13,
})


def stem_plot(ax, x, y, title, color):
    markerline, stemlines, baseline = ax.stem(x, y, basefmt='k-')
    plt.setp(markerline, markersize=7, markerfacecolor=color, markeredgecolor=color)
    plt.setp(stemlines, linewidth=1.8, color=color)
    plt.setp(baseline, linewidth=1.0, color='0.25')
    ax.set_title(title)
    ax.set_xlabel('n')
    ax.set_ylabel('Amplitude')
    ax.set_xticks(np.arange(int(np.min(x)), int(np.max(x)) + 1))
    ax.grid(True, linestyle='--', alpha=0.35)


def save_two_panel(filename, left_data, left_title, right_x, right_data, right_title, right_color='tab:orange'):
    fig, axes = plt.subplots(1, 2, figsize=(11.5, 4.4), constrained_layout=True)
    stem_plot(axes[0], indices, left_data, left_title, 'tab:blue')
    stem_plot(axes[1], right_x, right_data, right_title, right_color)
    ymin = min(np.min(left_data), np.min(right_data)) - 1
    ymax = max(np.max(left_data), np.max(right_data)) + 1
    for ax in axes:
        ax.set_ylim(ymin, ymax)
    fig.savefig(OUTPUT_DIR / filename, dpi=220, bbox_inches='tight')
    plt.close(fig)


def save_three_panel(filename, first_title, first_data, second_title, second_data, third_title, third_data):
    fig, axes = plt.subplots(1, 3, figsize=(15, 4.4), constrained_layout=True)
    stem_plot(axes[0], indices, first_data, first_title, 'tab:blue')
    stem_plot(axes[1], indices, second_data, second_title, 'tab:green')
    stem_plot(axes[2], indices, third_data, third_title, 'tab:red')
    ymin = min(np.min(first_data), np.min(second_data), np.min(third_data)) - 1
    ymax = max(np.max(first_data), np.max(second_data), np.max(third_data)) + 1
    for ax in axes:
        ax.set_ylim(ymin, ymax)
    fig.savefig(OUTPUT_DIR / filename, dpi=220, bbox_inches='tight')
    plt.close(fig)


save_two_panel(
    'reflexao.png',
    signal,
    'Sinal original x[n]',
    indices,
    reflected,
    'Sinal refletido x[-n]'
)

save_two_panel(
    'decimacao.png',
    signal,
    'Sinal original x[n]',
    np.arange(len(decimated)),
    decimated,
    f'Decimacao (fator = {factor_dec})',
    right_color='tab:purple'
)

save_two_panel(
    'amplitude.png',
    signal,
    'Sinal original x[n]',
    indices,
    amplified,
    f'Alteracao de amplitude (a = {factor_amp:g})',
    right_color='tab:orange'
)

save_two_panel(
    'deslocamento.png',
    signal,
    'Sinal original x[n]',
    indices,
    shifted,
    f'Deslocamento em janela fixa (k = {shift_k})',
    right_color='tab:brown'
)

save_two_panel(
    'deslocamento_dominio_variavel.png',
    signal,
    'Sinal original x[n]',
    shifted_domain_indices,
    shifted_domain_signal,
    f'Dominio deslocado (k = {shift_k})',
    right_color='tab:olive'
)

save_three_panel(
    'degrau_soma.png',
    'Sinal original x[n]',
    signal,
    f'Degrau (A = {step_amp:g})',
    step_signal,
    'Soma x[n] + u[n]',
    summed,
)

print('Figuras geradas em', OUTPUT_DIR)