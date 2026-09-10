import os
import csv
import matplotlib.pyplot as plt
import numpy as np

# Estilo tipográfico limpo e acadêmico
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['font.size'] = 10
plt.rcParams['axes.titlesize'] = 11
plt.rcParams['axes.titleweight'] = 'bold'
plt.rcParams['axes.labelsize'] = 10
plt.rcParams['axes.labelweight'] = 'bold'
plt.rcParams['xtick.labelsize'] = 9
plt.rcParams['ytick.labelsize'] = 9
plt.rcParams['legend.fontsize'] = 9
plt.rcParams['figure.titlesize'] = 12
plt.rcParams['lines.linewidth'] = 2.0
plt.rcParams['lines.markersize'] = 6

os.makedirs('imagens', exist_ok=True)
res_dir = 'codigo/benchmark/resultados'

CORES = {
    'Trie': '#1f77b4',
    'Patricia': '#ff7f0e',
    'AVL': '#2ca02c',
    'BST': '#d62728',
    'Splay': '#9467bd',
    'Treap': '#8c564b',
    'KDTree': '#17becf',
    'Linear': '#7f7f7f'
}

# ============================================================================
# FIGURA 1: CENÁRIO 1 - Prefixos e Conjuntos de Strings
# ============================================================================
def plot_cenario1():
    csv_path = os.path.join(res_dir, 'cenario1_strings.csv')
    if not os.path.exists(csv_path):
        print(f"Aviso: {csv_path} nao encontrado.")
        return

    dados = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            est = row['Estrutura']
            if est not in dados:
                dados[est] = {'N': [], 'TempoIns': [], 'TempoBus': [], 'Nos': []}
            dados[est]['N'].append(int(row['N']))
            dados[est]['TempoIns'].append(float(row['TempoInsercaoMs']))
            dados[est]['TempoBus'].append(float(row['TempoBuscaMs']))
            dados[est]['Nos'].append(int(row['QtdNos']))

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4.2), dpi=300)

    # Subplot 1: Tempo de Inserção vs N
    for est in ['Trie', 'Patricia', 'AVL', 'BST']:
        if est in dados:
            ax1.plot(dados[est]['N'], dados[est]['TempoIns'], marker='o', label=est, color=CORES.get(est, 'black'))

    ax1.set_title('(a) Tempo de Inserção de Strings')
    ax1.set_xlabel('Quantidade de Chaves (N)')
    ax1.set_ylabel('Tempo (ms)')
    ax1.grid(True, linestyle='--', alpha=0.6)
    ax1.legend()

    # Subplot 2: Contagem de Nós (Trie vs Patricia)
    for est in ['Trie', 'Patricia']:
        if est in dados:
            nos_k = [x / 1000.0 for x in dados[est]['Nos']]
            ax2.plot(dados[est]['N'], nos_k, marker='s', label=f'{est} (k nós)', color=CORES.get(est, 'black'))

    ax2.set_title('(b) Compressão Estrutural de Nós')
    ax2.set_xlabel('Quantidade de Chaves (N)')
    ax2.set_ylabel('Nós Alocados (x10³)')
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.legend()

    plt.tight_layout()
    out_path = 'imagens/cenario1_prefixos.png'
    plt.savefig(out_path, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gerado: {out_path}")

# ============================================================================
# FIGURA 2: CENÁRIO 2 - Inserção Ordenada Patológica
# ============================================================================
def plot_cenario2():
    csv_path = os.path.join(res_dir, 'cenario2_ordenada.csv')
    if not os.path.exists(csv_path):
        print(f"Aviso: {csv_path} nao encontrado.")
        return

    dados = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            est = row['Estrutura']
            if est not in dados:
                dados[est] = {'N': [], 'Altura': [], 'TempoIns': [], 'TempoBus': []}
            dados[est]['N'].append(int(row['N']))
            dados[est]['Altura'].append(int(row['Altura']))
            dados[est]['TempoIns'].append(float(row['TempoInsercaoMs']))
            dados[est]['TempoBus'].append(float(row['TempoBuscaMs']))

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4.2), dpi=300)

    # Subplot 1: Altura da Árvore vs N
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            ax1.plot(dados[est]['N'], dados[est]['Altura'], marker='o', label=est, color=CORES.get(est, 'black'))

    ax1.set_title('(a) Altura da Árvore (Degeneração)')
    ax1.set_xlabel('Elementos Inseridos em Ordem (N)')
    ax1.set_ylabel('Altura')
    ax1.set_yscale('log')
    ax1.grid(True, linestyle='--', alpha=0.6, which='both')
    ax1.legend()

    # Subplot 2: Tempo de Inserção Acumulado (ms)
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            ax2.plot(dados[est]['N'], dados[est]['TempoIns'], marker='s', label=est, color=CORES.get(est, 'black'))

    ax2.set_title('(b) Tempo de Inserção Ordenada')
    ax2.set_xlabel('Elementos Inseridos em Ordem (N)')
    ax2.set_ylabel('Tempo de Inserção (ms)')
    ax2.set_yscale('log')
    ax2.grid(True, linestyle='--', alpha=0.6, which='both')
    ax2.legend()

    plt.tight_layout()
    out_path = 'imagens/cenario2_altura_ordenada.png'
    plt.savefig(out_path, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gerado: {out_path}")

# ============================================================================
# FIGURA 3: CENÁRIO 3 - Localidade Temporal e Zipf
# ============================================================================
def plot_cenario3():
    csv_path = os.path.join(res_dir, 'cenario3_localidade.csv')
    if not os.path.exists(csv_path):
        print(f"Aviso: {csv_path} nao encontrado.")
        return

    dist_map = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            d = row['Distribuicao']
            est = row['Estrutura']
            t = float(row['TempoTotalMs'])
            if d not in dist_map:
                dist_map[d] = {}
            dist_map[d][est] = t

    fig, ax = plt.subplots(figsize=(7.2, 4.0), dpi=300)

    distribuicoes = list(dist_map.keys())
    estruturas = ['Splay', 'AVL', 'Treap']
    x = np.arange(len(distribuicoes))
    width = 0.25

    for idx, est in enumerate(estruturas):
        vals = [dist_map[d].get(est, 0.0) for d in distribuicoes]
        ax.bar(x + (idx - 1) * width, vals, width, label=est, color=CORES.get(est, 'gray'))

    ax.set_title('Latência Total em 100.000 Consultas (ms)')
    ax.set_xticks(x)
    ax.set_xticklabels(distribuicoes)
    ax.set_ylabel('Tempo Total (ms)')
    ax.grid(True, linestyle='--', alpha=0.5, axis='y')
    ax.legend(title='Estrutura')

    plt.tight_layout()
    out_path = 'imagens/cenario3_localidade_splay.png'
    plt.savefig(out_path, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gerado: {out_path}")

# ============================================================================
# FIGURA 4: CENÁRIO 4 - Maldição da Dimensionalidade
# ============================================================================
def plot_cenario4():
    csv_path = os.path.join(res_dir, 'cenario4_kdtree.csv')
    if not os.path.exists(csv_path):
        print(f"Aviso: {csv_path} nao encontrado.")
        return

    dims, t_kd, t_lin, perc_vis = [], [], [], []
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            dims.append(int(row['Dimensao']))
            t_kd.append(float(row['TempoKDTreeUs']))
            t_lin.append(float(row['TempoLinearUs']))
            perc_vis.append(float(row['NosVisitadosPerc']))

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 4.2), dpi=300)

    # Subplot 1: Tempo por Consulta (µs): KD-Tree vs Linear Scan
    ax1.plot(dims, t_kd, marker='o', label='KD-Tree (k-NN)', color=CORES['KDTree'])
    ax1.plot(dims, t_lin, marker='^', label='Força Bruta (Linear Scan)', color=CORES['Linear'], linestyle='--')
    ax1.set_title('(a) Tempo Médio por Consulta 1-NN')
    ax1.set_xlabel('Dimensão do Espaço (k)')
    ax1.set_ylabel('Tempo por Consulta (µs)')
    ax1.set_yscale('log')
    ax1.grid(True, linestyle='--', alpha=0.6, which='both')
    ax1.legend()

    # Subplot 2: Porcentagem de Nós Visitados na KD-Tree
    ax2.plot(dims, perc_vis, marker='s', color='#d62728', label='% Nós Inspecionados')
    ax2.axhline(100, color='gray', linestyle=':', alpha=0.7, label='100% (Varredura Completa)')
    ax2.set_title('(b) Poda Euclidiana vs Dimensão')
    ax2.set_xlabel('Dimensão do Espaço (k)')
    ax2.set_ylabel('Nós Visitados (%)')
    ax2.set_ylim(-2, 105)
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.legend()

    plt.tight_layout()
    out_path = 'imagens/cenario4_maldicao_kdtree.png'
    plt.savefig(out_path, bbox_inches='tight')
    plt.close()
    print(f"[OK] Gerado: {out_path}")

if __name__ == '__main__':
    print(">>> Gerando gráficos das baterias de benchmark...")
    plot_cenario1()
    plot_cenario2()
    plot_cenario3()
    plot_cenario4()
    print(">>> Todos os gráficos foram gerados em imagens/ com sucesso!")
