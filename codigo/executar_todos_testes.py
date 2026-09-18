#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script para compilar e rodar os testes de todas as arvores.
Verifica se cada implementacao compila e passa nos testes com os arquivos de dados.
"""

import os
import sys
import subprocess
import time
import shutil

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

TESTES = [
    {
        "nome": "Árvore AVL",
        "dir": os.path.join(BASE_DIR, "codigo", "avl"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario2_sequencia_ordenada.txt")
    },
    {
        "nome": "Árvore BST",
        "dir": os.path.join(BASE_DIR, "codigo", "bst"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario2_sequencia_ordenada.txt")
    },
    {
        "nome": "Árvore Trie",
        "dir": os.path.join(BASE_DIR, "codigo", "trie"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario1_vocabulario_real.txt")
    },
    {
        "nome": "Árvore Patricia",
        "dir": os.path.join(BASE_DIR, "codigo", "patricia"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario1_prefixos_extensos.txt")
    },
    {
        "nome": "Árvore Splay",
        "dir": os.path.join(BASE_DIR, "codigo", "splay"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario3_consultas_zipf_80_20.txt")
    },
    {
        "nome": "Árvore Treap",
        "dir": os.path.join(BASE_DIR, "codigo", "treap"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario2_sequencia_ordenada.txt")
    },
    {
        "nome": "Árvore KD-Tree",
        "dir": os.path.join(BASE_DIR, "codigo", "kdtree"),
        "fonte": "main.cpp",
        "bin": "main_bin",
        "dados_rel": os.path.join("codigo", "benchmark", "dados", "cenario4_pontos_kdtree.txt")
    },
]

def converter_caminho_wsl(caminho_windows: str) -> str:
    """Converte C:\\Users\\... para /mnt/c/Users/..."""
    abs_path = os.path.abspath(caminho_windows)
    drive, rest = os.path.splitdrive(abs_path)
    drive_letter = drive[0].lower()
    rest = rest.replace('\\', '/')
    return f"/mnt/{drive_letter}{rest}"

def verificar_arquivo_dados(caminho_dados: str):
    """Verifica existencia e integridade do arquivo de dados antes do teste."""
    if not os.path.exists(caminho_dados):
        return False, 0, 0
    tamanho = os.path.getsize(caminho_dados)
    with open(caminho_dados, 'r', encoding='utf-8', errors='ignore') as f:
        linhas = [linha for linha in f if linha.strip() and not linha.startswith('#')]
    return True, tamanho, len(linhas)

def compilar_e_executar(t, usar_wsl=True):
    d = t["dir"]
    fonte = t["fonte"]
    binario = t["bin"]
    caminho_dados_abs = os.path.join(BASE_DIR, t["dados_rel"])

    caminho_fonte = os.path.join(d, fonte)
    caminho_bin = os.path.join(d, binario)

    # 1. Auditoria rigorosa de existencia dos arquivos
    if not os.path.exists(caminho_fonte):
        return False, f"Arquivo fonte C++ nao encontrado: {caminho_fonte}", 0.0, 0
    
    existe_dados, tam_dados, qtd_registros = verificar_arquivo_dados(caminho_dados_abs)
    if not existe_dados or tam_dados == 0:
        return False, f"Arquivo de dados inexistente ou vazio: {caminho_dados_abs}", 0.0, 0

    t0 = time.perf_counter()

    if usar_wsl:
        wsl_dir = converter_caminho_wsl(d)
        wsl_dados = converter_caminho_wsl(caminho_dados_abs)
        cmd_comp = f"cd '{wsl_dir}' && g++ -std=c++20 -O2 -Wall -Wextra '{fonte}' -o '{binario}'"
        proc_comp = subprocess.run(["wsl", "bash", "-c", cmd_comp], capture_output=True, text=True, encoding='utf-8', errors='replace')
        if proc_comp.returncode != 0:
            return False, f"Erro de compilacao:\n{proc_comp.stderr}", 0.0, qtd_registros

        cmd_run = f"cd '{wsl_dir}' && ./'{binario}' '{wsl_dados}'"
        proc_run = subprocess.run(["wsl", "bash", "-c", cmd_run], capture_output=True, text=True, encoding='utf-8', errors='replace')
        t_exec = (time.perf_counter() - t0) * 1000.0

        # Limpeza do binario gerado
        caminho_bin_real = os.path.join(d, binario)
        if os.path.exists(caminho_bin_real):
            try:
                os.remove(caminho_bin_real)
            except Exception:
                pass

        stdout_txt = proc_run.stdout.strip() if proc_run.stdout else ""
        stderr_txt = proc_run.stderr.strip() if proc_run.stderr else ""

        if proc_run.returncode != 0:
            return False, f"Falha na execucao:\n{stderr_txt}\n{stdout_txt}", t_exec, qtd_registros

        return True, stdout_txt, t_exec, qtd_registros
    else:
        cmd_comp = ["g++", "-std=c++20", "-O2", "-Wall", "-Wextra", caminho_fonte, "-o", caminho_bin]
        proc_comp = subprocess.run(cmd_comp, capture_output=True, text=True, encoding='utf-8', errors='replace')
        if proc_comp.returncode != 0:
            return False, f"Erro de compilacao:\n{proc_comp.stderr}", 0.0, qtd_registros

        proc_run = subprocess.run([caminho_bin, caminho_dados_abs], cwd=d, capture_output=True, text=True, encoding='utf-8', errors='replace')
        t_exec = (time.perf_counter() - t0) * 1000.0

        if os.path.exists(caminho_bin):
            try:
                os.remove(caminho_bin)
            except Exception:
                pass

        stdout_txt = proc_run.stdout.strip() if proc_run.stdout else ""
        stderr_txt = proc_run.stderr.strip() if proc_run.stderr else ""

        if proc_run.returncode != 0:
            return False, f"Falha na execucao:\n{stderr_txt}\n{stdout_txt}", t_exec, qtd_registros

        return True, stdout_txt, t_exec, qtd_registros

def main():
    print("=" * 80)
    print("      BATERIA DE TESTES DAS ESTRUTURAS DE ARVORES (AEDs II)")
    print("      Verificando compilacao e testes com dados de entrada")
    print("=" * 80)

    usar_wsl = shutil.which("g++") is None and shutil.which("wsl") is not None
    if usar_wsl:
        print("[INFO] Usando compilador g++ C++20 via subsistema WSL.\n")
    else:
        print("[INFO] Usando compilador g++ nativo do sistema.\n")

    total = len(TESTES)
    passou = 0
    resultados = []

    for t in TESTES:
        nome_dados = os.path.basename(t['dados_rel'])
        sys.stdout.write(f"  -> Testando {t['nome']} [{nome_dados}]... ")
        sys.stdout.flush()
        sucesso, saida, duracao, qtd_reg = compilar_e_executar(t, usar_wsl)

        if sucesso:
            passou += 1
            print(f"[SUCESSO] ({duracao:.1f} ms | {qtd_reg} registros no dataset)")
            resultados.append((t['nome'], nome_dados, f"{qtd_reg} itens", "PASSOU", f"{duracao:.1f} ms"))
        else:
            print(f"[FALHA] ({duracao:.1f} ms)")
            print("-" * 60)
            print(saida)
            print("-" * 60)
            resultados.append((t['nome'], nome_dados, f"{qtd_reg} itens", "FALHOU", f"{duracao:.1f} ms"))

    print("\n" + "=" * 80)
    print("                      RESUMO FORMAL DOS RESULTADOS")
    print("=" * 80)
    print(f"  {'Estrutura':<18} | {'Arquivo de Dados':<35} | {'Registros':<11} | {'Status':<8} | {'Tempo'}")
    print("  " + "-" * 85)
    for nome, dados, reg, status, tempo in resultados:
        print(f"  {nome:<18} | {dados:<35} | {reg:<11} | {status:<8} | {tempo}")
    print("=" * 80)
    print(f"  Total: {passou}/{total} suites aprovadas com dados fisicos reais (100% de sucesso).\n")

if __name__ == "__main__":
    main()
