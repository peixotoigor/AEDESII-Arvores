#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para executar os benchmarks completos das arvores:
1. Verifica dependencias (compilador C++, Python, matplotlib).
2. Gera dados de teste caso nao existam.
3. Compila o benchmark em C++.
4. Roda os experimentos.
5. Gera os graficos.
"""

import os
import sys
import shutil
import subprocess
import time

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..', '..')) if os.path.exists(os.path.join(SCRIPT_DIR, 'dados')) else os.getcwd()
BENCHMARK_DIR = os.path.join(ROOT_DIR, 'codigo', 'benchmark')
DADOS_DIR = os.path.join(BENCHMARK_DIR, 'dados')
RES_DIR = os.path.join(BENCHMARK_DIR, 'resultados')
IMG_DIR = os.path.join(ROOT_DIR, 'imagens')
IMG_SEP_DIR = os.path.join(IMG_DIR, 'separadas')


def imprimir_cabecalho():
    print("=" * 78)
    print("  BATERIA DE BENCHMARKS - ESTRUTURAS DE ARVORES (AEDs II)")
    print("  Estruturas: Trie, Patricia, Splay, Treap, KD-Tree (com BST e AVL)")
    print("=" * 78)


def verificar_dependencias():
    print("\n[ETAPA 1/5] Verificando dependências do sistema...")
    # 1. Compilador C++
    compilador = None
    for c in ['g++', 'clang++', 'cl']:
        if shutil.which(c):
            compilador = c
            break
    
    # Se estiver no Windows sem g++ nativo, verificar se WSL possui g++
    usa_wsl = False
    if not compilador and sys.platform.startswith('win'):
        if shutil.which('wsl'):
            ret = subprocess.run(['wsl', 'which', 'g++'], capture_output=True, text=True)
            if ret.returncode == 0:
                compilador = 'g++ (via WSL)'
                usa_wsl = True

    if not compilador:
        print("[ERRO FATAL] Nenhum compilador C++20 detectado (g++, clang++ ou WSL g++).")
        sys.exit(1)

    print(f"  -> Compilador C++: {compilador}")

    # 2. Bibliotecas Python
    try:
        import matplotlib
        import numpy
        print(f"  -> Python: {sys.version.split()[0]} | Matplotlib {matplotlib.__version__} | NumPy {numpy.__version__}")
    except ImportError as e:
        print(f"[ERRO FATAL] Dependência ausente: {e}. Instale com: pip install matplotlib numpy")
        sys.exit(1)

    return compilador, usa_wsl


def verificar_datasets():
    print("\n[ETAPA 2/5] Verificando datasets de entrada em codigo/benchmark/dados/...")
    arquivos_esperados = [
        "cenario1_vocabulario_real.txt",
        "cenario1_prefixos_extensos.txt",
        "cenario2_sequencia_ordenada.txt",
        "cenario3_consultas_zipf_80_20.txt",
        "cenario3_consultas_uniforme.txt",
        "cenario4_pontos_kdtree.txt"
    ]

    todos_presentes = True
    for arq in arquivos_esperados:
        p = os.path.join(DADOS_DIR, arq)
        if not os.path.exists(p) or os.path.getsize(p) == 0:
            todos_presentes = False
            break

    if not todos_presentes:
        print("  -> Datasets incompletos. Executando gerador determinístico (gerar_dados_benchmark.py)...")
        script_dados = os.path.join(BENCHMARK_DIR, 'gerar_dados_benchmark.py')
        ret = subprocess.run([sys.executable, script_dados], cwd=ROOT_DIR)
        if ret.returncode != 0:
            print("[ERRO] Falha ao gerar datasets.")
            sys.exit(1)
        print("  -> Datasets gerados com sucesso.")
    else:
        print("  -> Todos os 6 conjuntos de dados físicos foram verificados com sucesso.")


def converter_caminho_wsl(caminho_windows: str) -> str:
    r"""Converte caminho Windows (C:\...) para formato WSL (/mnt/c/...)."""
    abs_path = os.path.abspath(caminho_windows)
    drive, rest = os.path.splitdrive(abs_path)
    drive_letter = drive[0].lower()
    rest = rest.replace('\\', '/')
    return f"/mnt/{drive_letter}{rest}"


def compilar_benchmark(compilador, usa_wsl):
    print("\n[ETAPA 3/5] Compilando benchmark.cpp com otimização -O3 -march=native...")
    bin_path = os.path.join(BENCHMARK_DIR, 'benchmark_bin')
    cpp_path = os.path.join(BENCHMARK_DIR, 'benchmark.cpp')

    if usa_wsl:
        wsl_root = converter_caminho_wsl(ROOT_DIR)
        cmd = f"cd '{wsl_root}' && g++ -std=c++20 -O3 -march=native codigo/benchmark/benchmark.cpp -o codigo/benchmark/benchmark_bin"
        ret = subprocess.run(['wsl', 'bash', '-c', cmd])
    else:
        cmd = [compilador, '-std=c++20', '-O3', '-march=native', cpp_path, '-o', bin_path]
        ret = subprocess.run(cmd, cwd=ROOT_DIR)

    if ret.returncode != 0:
        print("[ERRO] Falha na compilação do benchmark C++.")
        sys.exit(1)

    print("  -> Binário compilado com sucesso: codigo/benchmark/benchmark_bin")


def executar_bateria_benchmarks(usa_wsl):
    print("\n[ETAPA 4/5] Executando bateria de testes com 10 repetições independentes...")
    t_inicio = time.time()

    if usa_wsl:
        wsl_root = converter_caminho_wsl(ROOT_DIR)
        cmd = f"cd '{wsl_root}' && ./codigo/benchmark/benchmark_bin"
        ret = subprocess.run(['wsl', 'bash', '-c', cmd])
    else:
        bin_path = os.path.join(BENCHMARK_DIR, 'benchmark_bin')
        ret = subprocess.run([bin_path], cwd=ROOT_DIR)

    t_total = time.time() - t_inicio
    if ret.returncode != 0:
        print("[ERRO] Falha durante a execução dos benchmarks.")
        sys.exit(1)

    print(f"  -> Bateria concluída em {t_total:.1f} segundos.")
    print("  -> Arquivos CSV atualizados em: codigo/benchmark/resultados/")


def gerar_graficos():
    print("\n[ETAPA 5/5] Gerando gráficos científicos (compostos e separados)...")
    script_graficos = os.path.join(BENCHMARK_DIR, 'gerar_graficos.py')
    ret = subprocess.run([sys.executable, script_graficos], cwd=ROOT_DIR)
    if ret.returncode != 0:
        print("[ERRO] Falha na geração dos gráficos.")
        sys.exit(1)


def main():
    imprimir_cabecalho()
    compilador, usa_wsl = verificar_dependencias()
    verificar_datasets()
    compilar_benchmark(compilador, usa_wsl)
    executar_bateria_benchmarks(usa_wsl)
    gerar_graficos()

    print("\n" + "=" * 78)
    print("  [SUCESSO] Pipeline completo de reprodução executado com êxito!")
    print(f"  Resultados numéricos: {RES_DIR}")
    print(f"  Painéis do Artigo:   {IMG_DIR}")
    print(f"  Imagens Separadas:   {IMG_SEP_DIR}")
    print("=" * 78 + "\n")


if __name__ == '__main__':
    main()
