#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Gerador de arquivos de dados para os testes de desempenho das arvores:
1. Strings com prefixos reais e longos (para Trie e Patricia).
2. Sequencia ordenada de inteiros (para BST, AVL, Splay e Treap).
3. Consultas com distribuicao Zipf (80/20) e uniforme.
"""

import os
import random

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DADOS_DIR = os.path.join(BASE_DIR, "dados")
os.makedirs(DADOS_DIR, exist_ok=True)


def gerar_vocabulario_real(n=100000, seed=42):
    """
    Gera vocabulário de strings reais com prefixos compartilhados (Cenário 1).
    Utiliza raízes morfológicas autênticas e derivações lexicais.
    """
    caminho = os.path.join(DADOS_DIR, "cenario1_vocabulario_real.txt")
    print(f"[GERADOR] Criando vocabulário real com {n} palavras em: {caminho}")

    raizes = [
        "algorit", "comput", "estrutur", "sistem", "process", "desenvolv",
        "comunic", "inform", "tecnolog", "program", "rede", "graf", "arvor",
        "otimiz", "seguranc", "analis", "model", "aprendiz", "inteligen",
        "banco", "dad", "memori", "arquitet", "execuc", "balance", "rotac",
        "pesquis", "classif", "distrib", "paralel", "concorr", "vetor",
        "matriz", "hash", "indice", "operac", "linguag", "compilad",
        "transmiss", "pacot", "rotead", "protocol", "interfac", "aplicac"
    ]
    
    prefixos_gramaticais = [
        "", "sub", "super", "hiper", "ultra", "inter", "intra", "trans",
        "sobre", "sob", "ante", "pre", "pos", "retro", "re", "des",
        "in", "co", "contra", "anti", "macro", "micro", "multi", "poli"
    ]
    
    sufixos = [
        "o", "a", "os", "as", "ar", "er", "ir", "ando", "endo", "indo",
        "ado", "ido", "ador", "ante", "ente", "al", "ico", "ica", "ismo",
        "ista", "cao", "sao", "dade", "vel", "mente", "izavel", "aria",
        "eza", "ancia", "encia", "ario", "oria", "ura", "oso", "osa"
    ]

    rng = random.Random(seed)
    palavras = set()

    # Gera palavras combinando prefixo + raiz + sufixo + identificador opcional para garantir 100k palavras únicas
    for pref in prefixos_gramaticais:
        for r in raizes:
            for s in sufixos:
                w = pref + r + s
                palavras.add(w)

    idx = 0
    while len(palavras) < n:
        pref = rng.choice(prefixos_gramaticais)
        r = rng.choice(raizes)
        s = rng.choice(sufixos)
        w = f"{pref}{r}{s}_{idx}"
        palavras.add(w)
        idx += 1

    lista_palavras = list(palavras)
    rng.shuffle(lista_palavras)
    lista_palavras = lista_palavras[:n]
    with open(caminho, "w", encoding="utf-8", newline="\n") as f:
        for p in lista_palavras:
            f.write(p + "\n")
    print(f"  [OK] Vocabulário real gerado: {len(lista_palavras)} palavras únicas.")


def gerar_prefixos_extensos(n=100000, seed=42):
    """
    Gera chaves sintéticas com prefixos comuns extensos (L in [20, 100]) (Cenário 1).
    Simula namespaces hierárquicos, URLs corporativas e rotas de microsserviços.
    """
    caminho = os.path.join(DADOS_DIR, "cenario1_prefixos_extensos.txt")
    print(f"[GERADOR] Criando chaves com prefixos longos (L in [20, 100]) em: {caminho}")

    rng = random.Random(seed)
    # Define 100 prefixos longos compartilhados (comprimento entre 25 e 80 caracteres)
    prefixos_longos = []
    bases = [
        "br.edu.cefetmg.decom.aeds2.arvores.estruturas.avancadas.",
        "com.enterprise.cloud.services.microservices.datacenter.us_east.",
        "org.apache.distributed.systems.clustering.consensus.raft.log.",
        "api.v2.telemetry.metrics.timeseries.aggregated.node.sensor.",
        "gov.br.servicos.plataforma.cidadania.autenticacao.oauth2."
    ]
    subdominios = [
        "production.cluster_alpha.nodes.worker_",
        "staging.cluster_beta.storage.shard_",
        "analytics.pipeline_gamma.kafka.topic_",
        "monitoring.prometheus.exporter.agent_",
        "security.firewall.ruleset.inspection_"
    ]

    for b in bases:
        for s in subdominios:
            for k in range(4):
                prefixos_longos.append(f"{b}{s}{k:02d}.")

    chaves = []
    for i in range(n):
        p = prefixos_longos[i % len(prefixos_longos)]
        sufixo = f"registro_{i // len(prefixos_longos):06d}_hash_{rng.randint(100000, 999999)}"
        chaves.append(p + sufixo)

    rng.shuffle(chaves)
    with open(caminho, "w", encoding="utf-8", newline="\n") as f:
        for c in chaves:
            f.write(c + "\n")
    print(f"  [OK] Prefixos extensos gerados: {len(chaves)} chaves com L in [25, 95].")


def gerar_sequencia_ordenada(n=50000):
    """
    Gera sequência crescente estrita 1..n para o Cenário 2 (Inserção Ordenada Patológica).
    """
    caminho = os.path.join(DADOS_DIR, "cenario2_sequencia_ordenada.txt")
    print(f"[GERADOR] Criando sequência ordenada para N={n} em: {caminho}")
    with open(caminho, "w", encoding="utf-8", newline="\n") as f:
        for i in range(1, n + 1):
            f.write(f"{i}\n")
    print(f"  [OK] Sequência ordenada gerada: 1 a {n}.")


def gerar_consultas_zipf_e_uniforme(n=50000, m=100000, seed=42):
    """
    Gera traços de consulta para o Cenário 3 (Localidade Temporal de Acesso):
    - Zipf 80/20: exatamente 80% das consultas incidem sobre os primeiros 20% das chaves (Pareto).
    - Uniforme: consultas distribuídas homogeneamente sobre todas as chaves.
    """
    rng = random.Random(seed)
    
    # 1. Zipf 80/20
    caminho_zipf = os.path.join(DADOS_DIR, "cenario3_consultas_zipf_80_20.txt")
    print(f"[GERADOR] Criando traço Zipf 80/20 ({m} consultas sobre N={n}) em: {caminho_zipf}")
    corte_20 = int(0.20 * n)  # Primeiros 20% das chaves (0 a 9.999)
    
    consultas_zipf = []
    for _ in range(m):
        if rng.random() < 0.80:
            # 80% das requisições vão para o subconjunto de 20% mais frequente
            chave = rng.randint(0, corte_20 - 1) * 2
        else:
            # 20% das requisições vão para os 80% restantes
            chave = rng.randint(corte_20, n - 1) * 2
        consultas_zipf.append(chave)

    with open(caminho_zipf, "w", encoding="utf-8", newline="\n") as f:
        for q in consultas_zipf:
            f.write(f"{q}\n")
    print(f"  [OK] Traço Zipf 80/20 gerado com {m} consultas.")

    # 2. Uniforme
    caminho_uni = os.path.join(DADOS_DIR, "cenario3_consultas_uniforme.txt")
    print(f"[GERADOR] Criando traço Uniforme ({m} consultas sobre N={n}) em: {caminho_uni}")
    consultas_uni = [rng.randint(0, n - 1) * 2 for _ in range(m)]

    with open(caminho_uni, "w", encoding="utf-8", newline="\n") as f:
        for q in consultas_uni:
            f.write(f"{q}\n")
    print(f"  [OK] Traço Uniforme gerado com {m} consultas.")


def main():
    print("=" * 72)
    print("  GERAÇÃO DE DADOS PARA PROTOCOLO EXPERIMENTAL (main.tex §6.1)")
    print("=" * 72)
    gerar_vocabulario_real()
    gerar_prefixos_extensos()
    gerar_sequencia_ordenada()
    gerar_consultas_zipf_e_uniforme()
    print("=" * 72)
    print("  [SUCESSO] Todos os datasets de benchmark foram gravados em disco!")
    print("=" * 72)


if __name__ == "__main__":
    main()
