#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para gerar os graficos dos experimentos das arvores.
Gera os graficos comparativos na pasta imagens/.
"""

import os
import csv
import matplotlib.pyplot as plt
import numpy as np

# Configurações globais de tipografia e estilo acadêmico limpo
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['font.size'] = 9.5
plt.rcParams['axes.titlesize'] = 10.5
plt.rcParams['axes.titleweight'] = 'bold'
plt.rcParams['axes.labelsize'] = 9.5
plt.rcParams['axes.labelweight'] = 'bold'
plt.rcParams['xtick.labelsize'] = 8.5
plt.rcParams['ytick.labelsize'] = 8.5
plt.rcParams['legend.fontsize'] = 8.5
plt.rcParams['lines.linewidth'] = 1.8
plt.rcParams['lines.markersize'] = 6.0
plt.rcParams['axes.spines.top'] = False
plt.rcParams['axes.spines.right'] = False
plt.rcParams['axes.edgecolor'] = '#444444'
plt.rcParams['axes.linewidth'] = 0.9

# Resolução dinâmica de caminhos independentemente do diretório de chamada
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# Se executado da raiz ou de dentro de codigo/benchmark:
if os.path.exists(os.path.join(SCRIPT_DIR, 'resultados')):
    ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..', '..'))
else:
    ROOT_DIR = os.path.abspath(os.getcwd())

RES_DIR = os.path.join(ROOT_DIR, 'codigo', 'benchmark', 'resultados')
IMG_DIR = os.path.join(ROOT_DIR, 'imagens')
IMG_SEP_DIR = os.path.join(IMG_DIR, 'separadas')

os.makedirs(IMG_DIR, exist_ok=True)
os.makedirs(IMG_SEP_DIR, exist_ok=True)

# Paleta profissional de alto contraste (compatível com Daltonismo e Escala de Cinza)
ESTILOS = {
    'Trie':       {'cor': '#1f77b4', 'estilo': '-',  'marker': 'o', 'nome': 'Árvore Trie'},
    'Patricia':   {'cor': '#d95f02', 'estilo': '--', 'marker': 's', 'nome': 'Árvore Patricia'},
    'AVL':        {'cor': '#2ca02c', 'estilo': '-',  'marker': '^', 'nome': 'Árvore AVL'},
    'BST':        {'cor': '#d62728', 'estilo': ':',  'marker': 'x', 'nome': 'Árvore BST'},
    'Splay':      {'cor': '#7570b3', 'estilo': '-.', 'marker': 'D', 'nome': 'Árvore Splay'},
    'Treap':      {'cor': '#8c564b', 'estilo': '--', 'marker': 'v', 'nome': 'Árvore Treap'},
    'KDTree':     {'cor': '#008080', 'estilo': '-',  'marker': 'o', 'nome': 'Árvore KD-Tree'},
    'Linear':     {'cor': '#555555', 'estilo': ':',  'marker': 's', 'nome': 'Busca Linear'}
}

# ============================================================================
# FIGURA 1: CENÁRIO 1 - Prefixos e Strings (Painel Triplo e Gráficos Isolados)
# ============================================================================
def plot_cenario1():
    csv_path = os.path.join(RES_DIR, 'cenario1_strings.csv')
    if not os.path.exists(csv_path):
        print(f"[AVISO] {csv_path} não encontrado.")
        return

    dados = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            if 'Dataset' in row and row['Dataset'] != 'PrefixosExtensos':
                continue
            est = row['Estrutura']
            if est not in dados:
                dados[est] = {
                    'N': [], 'TempoIns': [], 'TempoInsStd': [],
                    'TempoBus': [], 'TempoBusStd': [], 'Nos': []
                }
            dados[est]['N'].append(int(row['N']))
            dados[est]['TempoIns'].append(float(row['TempoInsercaoMs']))
            dados[est]['TempoInsStd'].append(float(row.get('TempoInsercaoStd', 0.0)))
            dados[est]['TempoBus'].append(float(row['TempoBuscaMs']))
            dados[est]['TempoBusStd'].append(float(row.get('TempoBuscaStd', 0.0)))
            dados[est]['Nos'].append(int(row['QtdNos']))

    # 1. Painel Composto (1x3) para o Artigo
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(14, 4.2), dpi=300)

    # Subplot 1: Tempo de Inserção de Strings
    for est in ['Trie', 'Patricia', 'AVL', 'BST']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoIns'])
            yerr = np.array(dados[est]['TempoInsStd'])
            ax1.errorbar(
                x, y, yerr=yerr,
                label=cfg.get('nome', est),
                color=cfg.get('cor', 'black'),
                linestyle=cfg.get('estilo', '-'),
                marker=cfg.get('marker', 'o'),
                capsize=3.5, capthick=1.0, elinewidth=1.0
            )
            ax1.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)

    ax1.set_title('(a) Tempo de Inserção de Chaves\n(Trie/Patricia escalam com tamanho da chave)', pad=8)
    ax1.set_xlabel('Quantidade de Chaves (N)')
    ax1.set_ylabel('Tempo de Inserção (ms)')
    ax1.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax1.legend(frameon=True, facecolor='white', framealpha=0.9)

    # Subplot 2: Tempo de Busca de Prefixos e Palavras
    for est in ['Trie', 'Patricia', 'AVL', 'BST']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoBus'])
            yerr = np.array(dados[est]['TempoBusStd'])
            ax2.errorbar(
                x, y, yerr=yerr,
                label=cfg.get('nome', est),
                color=cfg.get('cor', 'black'),
                linestyle=cfg.get('estilo', '-'),
                marker=cfg.get('marker', 'o'),
                capsize=3.5, capthick=1.0, elinewidth=1.0
            )
            ax2.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)

    ax2.set_title('(b) Tempo de Consulta (5.000 buscas)\n(Trie e Patricia mantêm latência O(k))', pad=8)
    ax2.set_xlabel('Quantidade de Chaves (N)')
    ax2.set_ylabel('Tempo de Consulta (ms)')
    ax2.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax2.legend(frameon=True, facecolor='white', framealpha=0.9)

    # Subplot 3: Contagem e Compressão de Nós (Trie vs Patricia)
    for est in ['Trie', 'Patricia']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            nos_k = [v / 1000.0 for v in dados[est]['Nos']]
            ax3.plot(
                dados[est]['N'], nos_k,
                label=cfg.get('nome', est),
                color=cfg.get('cor', 'black'),
                linestyle=cfg.get('estilo', '-'),
                marker=cfg.get('marker', 's')
            )

    ax3.set_title('(c) Compressão Estrutural de Nós\n(Patricia reduz ~91,5% dos nós da Trie)', pad=8)
    ax3.set_xlabel('Quantidade de Chaves (N)')
    ax3.set_ylabel('Nós Alocados (x10³ nós)')
    ax3.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax3.legend(frameon=True, facecolor='white', framealpha=0.9)

    plt.tight_layout()
    out_path = os.path.join(IMG_DIR, 'cenario1_prefixos.png')
    plt.savefig(out_path, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Painel Composto: {out_path}")

    # 2. Exportação de Gráficos Individuais Separados
    # 1a. Tempo de Inserção
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for est in ['Trie', 'Patricia', 'AVL', 'BST']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoIns'])
            yerr = np.array(dados[est]['TempoInsStd'])
            ax.errorbar(x, y, yerr=yerr, label=cfg.get('nome', est), color=cfg.get('cor', 'black'),
                         linestyle=cfg.get('estilo', '-'), marker=cfg.get('marker', 'o'), capsize=3.5)
            ax.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)
    ax.set_title('Cenário 1: Tempo de Inserção de Chaves Textuais')
    ax.set_xlabel('Quantidade de Chaves (N)')
    ax.set_ylabel('Tempo de Inserção (ms)')
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p1a = os.path.join(IMG_SEP_DIR, 'cenario1_a_tempo_insercao.png')
    plt.savefig(p1a, bbox_inches='tight', dpi=300)
    plt.close()

    # 1b. Tempo de Busca
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for est in ['Trie', 'Patricia', 'AVL', 'BST']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoBus'])
            yerr = np.array(dados[est]['TempoBusStd'])
            ax.errorbar(x, y, yerr=yerr, label=cfg.get('nome', est), color=cfg.get('cor', 'black'),
                         linestyle=cfg.get('estilo', '-'), marker=cfg.get('marker', 'o'), capsize=3.5)
            ax.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)
    ax.set_title('Cenário 1: Tempo de Busca de Prefixos e Chaves (5.000 buscas)')
    ax.set_xlabel('Quantidade de Chaves (N)')
    ax.set_ylabel('Tempo de Busca (ms)')
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p1b = os.path.join(IMG_SEP_DIR, 'cenario1_b_tempo_busca.png')
    plt.savefig(p1b, bbox_inches='tight', dpi=300)
    plt.close()

    # 1c. Nós Alocados
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for est in ['Trie', 'Patricia']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            nos_k = [v / 1000.0 for v in dados[est]['Nos']]
            ax.plot(dados[est]['N'], nos_k, label=cfg.get('nome', est), color=cfg.get('cor', 'black'),
                    linestyle=cfg.get('estilo', '-'), marker=cfg.get('marker', 's'))
    ax.set_title('Cenário 1: Contagem e Compactação de Nós (Trie vs Patricia)')
    ax.set_xlabel('Quantidade de Chaves (N)')
    ax.set_ylabel('Nós Alocados (x10³ nós)')
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p1c = os.path.join(IMG_SEP_DIR, 'cenario1_c_compressao_nos.png')
    plt.savefig(p1c, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Gráficos Separados Cenário 1: {IMG_SEP_DIR}")


# ============================================================================
# FIGURA 2: CENÁRIO 2 - Inserção Ordenada Patológica (BST vs AVL vs Treap vs Splay)
# ============================================================================
def plot_cenario2():
    csv_path = os.path.join(RES_DIR, 'cenario2_ordenada.csv')
    if not os.path.exists(csv_path):
        print(f"[AVISO] {csv_path} não encontrado.")
        return

    dados = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            est = row['Estrutura']
            if est not in dados:
                dados[est] = {
                    'N': [], 'Altura': [], 'AlturaStd': [],
                    'TempoIns': [], 'TempoInsStd': [],
                    'TempoBus': [], 'TempoBusStd': []
                }
            dados[est]['N'].append(int(row['N']))
            dados[est]['Altura'].append(float(row['Altura']))
            dados[est]['AlturaStd'].append(float(row.get('AlturaStd', 0.0)))
            dados[est]['TempoIns'].append(float(row['TempoInsercaoMs']))
            dados[est]['TempoInsStd'].append(float(row.get('TempoInsercaoStd', 0.0)))
            dados[est]['TempoBus'].append(float(row['TempoBuscaMs']))
            dados[est]['TempoBusStd'].append(float(row.get('TempoBuscaStd', 0.0)))

    # 1. Painel Composto (1x2) para o Artigo
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.4), dpi=300)

    # Subplot 1: Altura da Árvore vs N (Escala Logarítmica)
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['Altura'])
            ax1.plot(
                x, y,
                label=cfg.get('nome', est),
                color=cfg.get('cor', 'black'),
                linestyle=cfg.get('estilo', '-'),
                marker=cfg.get('marker', 'o')
            )

    ax1.set_title('(a) Altura da Árvore sob Inserção Ordenada\n(BST degenera para O(N); AVL/Treap garantem O(log N))', pad=8)
    ax1.set_xlabel('Elementos Inseridos em Sequência Ordenada (N)')
    ax1.set_ylabel('Altura da Árvore (níveis)')
    ax1.set_yscale('log')
    ax1.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb', which='both')
    ax1.legend(frameon=True, facecolor='white', framealpha=0.9)

    # Subplot 2: Tempo de Inserção Ordenada
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoIns'])
            yerr = np.array(dados[est]['TempoInsStd'])
            ax2.errorbar(
                x, y, yerr=yerr,
                label=cfg.get('nome', est),
                color=cfg.get('cor', 'black'),
                linestyle=cfg.get('estilo', '-'),
                marker=cfg.get('marker', 'o'),
                capsize=3.5, capthick=1.0, elinewidth=1.0
            )
            ax2.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)

    ax2.set_title('(b) Tempo de Construção Sequencial\n(Crescimento quadrático da BST inviabiliza grandes volumes)', pad=8)
    ax2.set_xlabel('Elementos Inseridos em Sequência Ordenada (N)')
    ax2.set_ylabel('Tempo de Inserção (ms)')
    ax2.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax2.legend(frameon=True, facecolor='white', framealpha=0.9)

    plt.tight_layout()
    out_path = os.path.join(IMG_DIR, 'cenario2_altura_ordenada.png')
    plt.savefig(out_path, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Painel Composto: {out_path}")

    # 2. Exportação de Gráficos Individuais Separados
    # 2a. Altura da Árvore
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            ax.plot(dados[est]['N'], dados[est]['Altura'], label=cfg.get('nome', est),
                    color=cfg.get('cor', 'black'), linestyle=cfg.get('estilo', '-'), marker=cfg.get('marker', 'o'))
    ax.set_title('Cenário 2: Altura da Árvore sob Inserção Ordenada (Níveis)')
    ax.set_xlabel('Elementos Inseridos (N)')
    ax.set_ylabel('Altura Máxima (níveis)')
    ax.set_yscale('log')
    ax.grid(True, linestyle='--', alpha=0.35, which='both')
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p2a = os.path.join(IMG_SEP_DIR, 'cenario2_a_altura_arvore.png')
    plt.savefig(p2a, bbox_inches='tight', dpi=300)
    plt.close()

    # 2b. Tempo de Inserção
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for est in ['BST', 'AVL', 'Treap', 'Splay']:
        if est in dados:
            cfg = ESTILOS.get(est, {})
            x = np.array(dados[est]['N'])
            y = np.array(dados[est]['TempoIns'])
            yerr = np.array(dados[est]['TempoInsStd'])
            ax.errorbar(x, y, yerr=yerr, label=cfg.get('nome', est), color=cfg.get('cor', 'black'),
                         linestyle=cfg.get('estilo', '-'), marker=cfg.get('marker', 'o'), capsize=3.5)
            ax.fill_between(x, np.maximum(0, y - yerr), y + yerr, color=cfg.get('cor', 'black'), alpha=0.1)
    ax.set_title('Cenário 2: Tempo de Construção sob Inserção Ordenada (ms)')
    ax.set_xlabel('Elementos Inseridos (N)')
    ax.set_ylabel('Tempo de Inserção (ms)')
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p2b = os.path.join(IMG_SEP_DIR, 'cenario2_b_tempo_insercao.png')
    plt.savefig(p2b, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Gráficos Separados Cenário 2: {IMG_SEP_DIR}")


# ============================================================================
# FIGURA 3: CENÁRIO 3 - Localidade Temporal e Zipf 80/20 (Splay vs AVL vs Treap)
# ============================================================================
def plot_cenario3():
    csv_path = os.path.join(RES_DIR, 'cenario3_localidade.csv')
    if not os.path.exists(csv_path):
        print(f"[AVISO] {csv_path} não encontrado.")
        return

    dados = {}
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            dist = row['Distribuicao']
            est = row['Estrutura']
            if dist not in dados:
                dados[dist] = {}
            dados[dist][est] = {
                'TempoTotal': float(row['TempoTotalMs']),
                'TempoTotalStd': float(row.get('TempoTotalStd', 0.0)),
                'Latencia': float(row['LatenciaMediaUs']),
                'LatenciaStd': float(row.get('LatenciaMediaStd', 0.0))
            }

    distribuicoes = ['Zipf (Localidade)', 'Uniforme']
    estruturas = ['Splay', 'AVL', 'Treap']
    x = np.arange(len(distribuicoes))
    largura = 0.25

    # 1. Painel Composto (1x2) para o Artigo
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.4), dpi=300)

    # Subplot 1: Tempo Total de Execução (ms)
    for i, est in enumerate(estruturas):
        tempos = [dados[d][est]['TempoTotal'] for d in distribuicoes]
        stds   = [dados[d][est]['TempoTotalStd'] for d in distribuicoes]
        cfg = ESTILOS.get(est, {})
        pos = x + (i - 1) * largura
        bars = ax1.bar(
            pos, tempos, largura, yerr=stds,
            label=cfg.get('nome', est),
            color=cfg.get('cor', 'black'),
            edgecolor='#222222', linewidth=0.8,
            capsize=4
        )
        for bar in bars:
            h = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2.0, h + 1.5, f'{h:.1f}', ha='center', va='bottom', fontsize=7.5)

    ax1.set_title('(a) Tempo Total para 100.000 Consultas\n(Splay se beneficia fortemente da localidade temporal)', pad=8)
    ax1.set_ylabel('Tempo Total (ms)')
    ax1.set_xticks(x)
    ax1.set_xticklabels(['Zipf (80% em 20% das chaves)', 'Uniforme (sem localidade)'])
    ax1.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb', axis='y')
    ax1.legend(frameon=True, facecolor='white', framealpha=0.9)

    # Subplot 2: Latência Média por Consulta (us/op)
    for i, est in enumerate(estruturas):
        lats = [dados[d][est]['Latencia'] for d in distribuicoes]
        stds = [dados[d][est]['LatenciaStd'] for d in distribuicoes]
        cfg = ESTILOS.get(est, {})
        pos = x + (i - 1) * largura
        bars = ax2.bar(
            pos, lats, largura, yerr=stds,
            label=cfg.get('nome', est),
            color=cfg.get('cor', 'black'),
            edgecolor='#222222', linewidth=0.8,
            capsize=4
        )
        for bar in bars:
            h = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2.0, h + 0.02, f'{h:.2f}', ha='center', va='bottom', fontsize=7.5)

    ax2.set_title('(b) Custo Médio por Operação de Busca\n(Na carga uniforme, o splaying gera sobrecarga moderada)', pad=8)
    ax2.set_ylabel('Latência Média (µs / consulta)')
    ax2.set_xticks(x)
    ax2.set_xticklabels(['Zipf (80% em 20% das chaves)', 'Uniforme (sem localidade)'])
    ax2.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb', axis='y')
    ax2.legend(frameon=True, facecolor='white', framealpha=0.9)

    plt.tight_layout()
    out_path = os.path.join(IMG_DIR, 'cenario3_localidade_splay.png')
    plt.savefig(out_path, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Painel Composto: {out_path}")

    # 2. Exportação de Gráficos Individuais Separados
    # 3a. Tempo Total
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for i, est in enumerate(estruturas):
        tempos = [dados[d][est]['TempoTotal'] for d in distribuicoes]
        stds   = [dados[d][est]['TempoTotalStd'] for d in distribuicoes]
        cfg = ESTILOS.get(est, {})
        pos = x + (i - 1) * largura
        bars = ax.bar(pos, tempos, largura, yerr=stds, label=cfg.get('nome', est),
                      color=cfg.get('cor', 'black'), edgecolor='#222222', capsize=4)
        for bar in bars:
            h = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2.0, h + 1.5, f'{h:.1f}', ha='center', va='bottom', fontsize=8)
    ax.set_title('Cenário 3: Tempo Total para 100.000 Consultas (ms)')
    ax.set_ylabel('Tempo Total (ms)')
    ax.set_xticks(x)
    ax.set_xticklabels(['Zipf (80/20)', 'Uniforme'])
    ax.grid(True, linestyle='--', alpha=0.35, axis='y')
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p3a = os.path.join(IMG_SEP_DIR, 'cenario3_a_tempo_total_zipf.png')
    plt.savefig(p3a, bbox_inches='tight', dpi=300)
    plt.close()

    # 3b. Latência Média
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    for i, est in enumerate(estruturas):
        lats = [dados[d][est]['Latencia'] for d in distribuicoes]
        stds = [dados[d][est]['LatenciaStd'] for d in distribuicoes]
        cfg = ESTILOS.get(est, {})
        pos = x + (i - 1) * largura
        bars = ax.bar(pos, lats, largura, yerr=stds, label=cfg.get('nome', est),
                      color=cfg.get('cor', 'black'), edgecolor='#222222', capsize=4)
        for bar in bars:
            h = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2.0, h + 0.02, f'{h:.2f}', ha='center', va='bottom', fontsize=8)
    ax.set_title('Cenário 3: Latência Média por Consulta (µs/op)')
    ax.set_ylabel('Latência Média (µs / consulta)')
    ax.set_xticks(x)
    ax.set_xticklabels(['Zipf (80/20)', 'Uniforme'])
    ax.grid(True, linestyle='--', alpha=0.35, axis='y')
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p3b = os.path.join(IMG_SEP_DIR, 'cenario3_b_latencia_media_zipf.png')
    plt.savefig(p3b, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Gráficos Separados Cenário 3: {IMG_SEP_DIR}")


# ============================================================================
# FIGURA 4: CENÁRIO 4 - Maldição da Dimensionalidade na KD-Tree
# ============================================================================
def plot_cenario4():
    csv_path = os.path.join(RES_DIR, 'cenario4_kdtree.csv')
    if not os.path.exists(csv_path):
        print(f"[AVISO] {csv_path} não encontrado.")
        return

    dim = []
    t_kdt, t_kdt_std = [], []
    t_lin, t_lin_std = [], []
    nos_vis, nos_vis_std = [], []

    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            dim.append(int(row['Dimensao']))
            t_kdt.append(float(row['TempoKDTreeUs']))
            t_kdt_std.append(float(row.get('TempoKDTreeStd', 0.0)))
            t_lin.append(float(row['TempoLinearUs']))
            t_lin_std.append(float(row.get('TempoLinearStd', 0.0)))
            nos_vis.append(float(row['NosVisitadosPerc']))
            nos_vis_std.append(float(row.get('NosVisitadosStd', 0.0)))

    # 1. Painel Composto (1x2) para o Artigo
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.4), dpi=300)

    cfg_kdt = ESTILOS['KDTree']
    cfg_lin = ESTILOS['Linear']

    ax1.errorbar(
        dim, t_kdt, yerr=t_kdt_std,
        label=cfg_kdt['nome'],
        color=cfg_kdt['cor'], linestyle=cfg_kdt['estilo'], marker=cfg_kdt['marker'],
        capsize=3.5, capthick=1.0, elinewidth=1.0
    )
    ax1.fill_between(dim, np.maximum(0, np.array(t_kdt) - np.array(t_kdt_std)),
                     np.array(t_kdt) + np.array(t_kdt_std), color=cfg_kdt['cor'], alpha=0.1)

    ax1.errorbar(
        dim, t_lin, yerr=t_lin_std,
        label=cfg_lin['nome'],
        color=cfg_lin['cor'], linestyle=cfg_lin['estilo'], marker=cfg_lin['marker'],
        capsize=3.5, capthick=1.0, elinewidth=1.0
    )
    ax1.fill_between(dim, np.maximum(0, np.array(t_lin) - np.array(t_lin_std)),
                     np.array(t_lin) + np.array(t_lin_std), color=cfg_lin['cor'], alpha=0.1)

    ax1.set_title('(a) Busca 1-NN: KD-Tree vs Varredura Linear\n(Para k > 10, a busca linear supera a KD-Tree)', pad=8)
    ax1.set_xlabel('Dimensão do Espaço Euclidiano (k)')
    ax1.set_ylabel('Tempo Médio por Consulta 1-NN (µs)')
    ax1.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax1.legend(frameon=True, facecolor='white', framealpha=0.9)

    ax2.errorbar(
        dim, nos_vis, yerr=nos_vis_std,
        label='Nós Percorridos na KD-Tree (%)',
        color='#c0392b', linestyle='-', marker='s',
        capsize=3.5, capthick=1.0, elinewidth=1.0
    )
    ax2.fill_between(dim, np.maximum(0, np.array(nos_vis) - np.array(nos_vis_std)),
                     np.minimum(100, np.array(nos_vis) + np.array(nos_vis_std)), color='#c0392b', alpha=0.12)
    ax2.axhline(100.0, color='gray', linestyle=':', label='Teto de Varredura Total (100%)')

    ax2.set_title('(b) Degeneração da Poda Geométrica\n(Em alta dimensão, quase todos os nós são inspecionados)', pad=8)
    ax2.set_xlabel('Dimensão do Espaço Euclidiano (k)')
    ax2.set_ylabel('Nós Visitados (% do total da base)')
    ax2.set_ylim(0, 105)
    ax2.grid(True, linestyle='--', alpha=0.35, color='#bbbbbb')
    ax2.legend(frameon=True, facecolor='white', framealpha=0.9)

    plt.tight_layout()
    out_path = os.path.join(IMG_DIR, 'cenario4_maldicao_kdtree.png')
    plt.savefig(out_path, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Painel Composto: {out_path}")

    # 2. Exportação de Gráficos Individuais Separados
    # 4a. Tempo 1-NN
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    ax.errorbar(dim, t_kdt, yerr=t_kdt_std, label=cfg_kdt['nome'], color=cfg_kdt['cor'],
                linestyle=cfg_kdt['estilo'], marker=cfg_kdt['marker'], capsize=3.5)
    ax.errorbar(dim, t_lin, yerr=t_lin_std, label=cfg_lin['nome'], color=cfg_lin['cor'],
                linestyle=cfg_lin['estilo'], marker=cfg_lin['marker'], capsize=3.5)
    ax.set_title('Cenário 4: Busca 1-NN (KD-Tree vs Varredura Linear)')
    ax.set_xlabel('Dimensão Euclidiana (k)')
    ax.set_ylabel('Tempo Médio (µs / consulta)')
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p4a = os.path.join(IMG_SEP_DIR, 'cenario4_a_knn_tempo_busca.png')
    plt.savefig(p4a, bbox_inches='tight', dpi=300)
    plt.close()

    # 4b. Poda geométrica
    fig, ax = plt.subplots(figsize=(6, 4.5), dpi=300)
    ax.errorbar(dim, nos_vis, yerr=nos_vis_std, label='Nós Visitados na KD-Tree (%)',
                color='#c0392b', linestyle='-', marker='s', capsize=3.5)
    ax.axhline(100.0, color='gray', linestyle=':', label='Varredura Total (100%)')
    ax.set_title('Cenário 4: Degeneração da Poda Espacial (% nós visitados)')
    ax.set_xlabel('Dimensão Euclidiana (k)')
    ax.set_ylabel('Nós Visitados (% do total da base)')
    ax.set_ylim(0, 105)
    ax.grid(True, linestyle='--', alpha=0.35)
    ax.legend(frameon=True, facecolor='white')
    plt.tight_layout()
    p4b = os.path.join(IMG_SEP_DIR, 'cenario4_b_percentual_nos_visitados.png')
    plt.savefig(p4b, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Gráficos Separados Cenário 4: {IMG_SEP_DIR}")


# ============================================================================
# FIGURA 5: VISUALIZAÇÃO DIDÁTICA DO PARTICIONAMENTO 2D DA KD-TREE
# ============================================================================
def plot_kdtree_particao_2d():
    pontos = [
        (5.0, 5.7, 'A'), (2.3, 3.0, 'B'), (7.4, 4.5, 'C'),
        (3.6, 1.4, 'D'), (7.5, 2.0, 'E'), (1.5, 6.8, 'F'),
        (6.2, 7.5, 'G'), (4.2, 4.2, 'H')
    ]

    fig, ax = plt.subplots(figsize=(6.2, 5.2), dpi=300)

    x_min, x_max = 0.0, 9.0
    y_min, y_max = 0.0, 9.0

    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)

    ax.plot([5.0, 5.0], [0.0, 9.0], color='#d62728', linestyle='-', linewidth=2.0, label='Corte no Eixo X (Nível Par)')
    ax.plot([0.0, 5.0], [3.0, 3.0], color='#1f77b4', linestyle='-', linewidth=2.0, label='Corte no Eixo Y (Nível Ímpar)')
    ax.plot([5.0, 9.0], [4.5, 4.5], color='#1f77b4', linestyle='-', linewidth=2.0)
    ax.plot([3.6, 3.6], [0.0, 3.0], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([1.5, 1.5], [3.0, 9.0], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([7.5, 7.5], [0.0, 4.5], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([6.2, 6.2], [4.5, 9.0], color='#d62728', linestyle='--', linewidth=1.5)

    for x, y, rotulo in pontos:
        ax.plot(x, y, 'ko', markersize=8, zorder=5)
        ax.text(x + 0.18, y + 0.18, f'{rotulo} ({x}, {y})', fontsize=8.5, fontweight='bold', zorder=6)

    q_x, q_y = 7.2, 4.2
    ax.plot(q_x, q_y, 'm*', markersize=12, label='Ponto de Consulta Q (7.2, 4.2)', zorder=7)
    raio = np.sqrt((7.2 - 7.4)**2 + (4.2 - 4.5)**2)
    circ = plt.Circle((q_x, q_y), raio, color='purple', fill=False, linestyle=':', linewidth=1.6, label='Raio de Busca 1-NN')
    ax.add_patch(circ)

    ax.set_title('Particionamento Recursivo do Espaço 2D pela KD-Tree\n(Alternância de eixos ortogonais e poda geométrica)', pad=10)
    ax.set_xlabel('Coordenada X')
    ax.set_ylabel('Coordenada Y')
    ax.grid(True, linestyle=':', alpha=0.3, color='#aaaaaa')
    ax.legend(loc='upper left', frameon=True, facecolor='white', framealpha=0.9, fontsize=8.0)

    plt.tight_layout()
    out_path = os.path.join(IMG_DIR, 'kdtree_particao_2d.png')
    plt.savefig(out_path, bbox_inches='tight', dpi=300)
    plt.close()

    # Cópia para imagens/separadas/
    out_sep = os.path.join(IMG_SEP_DIR, 'kdtree_particao_2d_individual.png')
    fig, ax = plt.subplots(figsize=(6.2, 5.2), dpi=300)
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.plot([5.0, 5.0], [0.0, 9.0], color='#d62728', linestyle='-', linewidth=2.0, label='Corte X (Nível Par)')
    ax.plot([0.0, 5.0], [3.0, 3.0], color='#1f77b4', linestyle='-', linewidth=2.0, label='Corte Y (Nível Ímpar)')
    ax.plot([5.0, 9.0], [4.5, 4.5], color='#1f77b4', linestyle='-', linewidth=2.0)
    ax.plot([3.6, 3.6], [0.0, 3.0], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([1.5, 1.5], [3.0, 9.0], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([7.5, 7.5], [0.0, 4.5], color='#d62728', linestyle='--', linewidth=1.5)
    ax.plot([6.2, 6.2], [4.5, 9.0], color='#d62728', linestyle='--', linewidth=1.5)
    for x, y, rotulo in pontos:
        ax.plot(x, y, 'ko', markersize=8, zorder=5)
        ax.text(x + 0.18, y + 0.18, f'{rotulo} ({x}, {y})', fontsize=8.5, fontweight='bold', zorder=6)
    ax.plot(q_x, q_y, 'm*', markersize=12, label='Consulta Q (7.2, 4.2)', zorder=7)
    circ = plt.Circle((q_x, q_y), raio, color='purple', fill=False, linestyle=':', linewidth=1.6, label='Raio 1-NN')
    ax.add_patch(circ)
    ax.set_title('Particionamento Recursivo do Espaço 2D pela KD-Tree')
    ax.set_xlabel('Coordenada X')
    ax.set_ylabel('Coordenada Y')
    ax.grid(True, linestyle=':', alpha=0.3)
    ax.legend(loc='upper left', frameon=True, facecolor='white', fontsize=8.0)
    plt.tight_layout()
    plt.savefig(out_sep, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"[OK] Particionamento 2D: {out_path} e {out_sep}")

def main():
    print("===============================================================")
    print("  GERAÇÃO DE GRÁFICOS CIENTÍFICOS - COMPOSTOS E SEPARADOS")
    print("===============================================================")
    plot_cenario1()
    plot_cenario2()
    plot_cenario3()
    plot_cenario4()
    plot_kdtree_particao_2d()
    print("===============================================================")
    print(f"  [CONCLUÍDO] Gráficos do artigo em: {IMG_DIR}")
    print(f"  [CONCLUÍDO] Gráficos individuais em: {IMG_SEP_DIR}")
    print("===============================================================")

if __name__ == '__main__':
    main()
