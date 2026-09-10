# AEDESII-Arvores: Estudo Comparativo e Empírico de Estruturas Hierárquicas Avançadas

Este repositório contém o trabalho prático individual da disciplina de **Algoritmos e Estruturas de Dados II (AEDES II)** do CEFET-MG. O projeto contempla o estudo teórico formal, implementação completa em C++20, rastreamento visual de algoritmos, análise assintótica e uma bateria abrangente de testes empíricos de desempenho para cinco estruturas de dados em árvore especializadas, comparadas contra referências clássicas (BST e AVL).

## Estruturas de Dados Estudadas

1. **Árvore Trie (Digital / Prefixo):** Decomposição caractere a caractere, nós multivalentes, ideal para autocompletar e dicionários com busca $\mathcal{O}(L)$.
2. **Árvore Patricia (Compacta de Prefixos):** Compressão de caminhos univalentes em arestas com fatias de string, reduzindo alocação de nós em até 86,8%.
3. **Árvore Splay (Autoajustável):** Rotações binárias amortizadas (*zig*, *zig-zig*, *zig-zag*) que adaptam dinamicamente a topologia para maximizar localidade temporal de acesso.
4. **Árvore Treap (Cartesiana / Aleatorizada):** Combinação elegante de Chaves BST e Prioridades de Min-Heap pseudoaleatórias, assegurando balanceamento estatístico imune a entradas ordenadas.
5. **KD-Tree (Multidimensional $k$-d):** Particionamento do espaço $\mathbb{R}^k$ por hiperplanos ortogonais ciclicamente alternados, viabilizando podas euclidianas e busca eficiente de vizinho mais próximo ($k$-NN).
6. **Baselines:** Árvore Binária de Busca clássica (BST) e Árvore AVL balanceada estritamente por fator de balanço $\in \{-1, 0, 1\}$.

---

## Organização do Repositório

```text
AEDESII-Arvores/
├── main.tex                    # Relatório científico acadêmico em LaTeX (12 páginas)
├── main.pdf                    # PDF compilado final do relatório
├── .gitignore                  # Regras de exclusão para compilação e binários
├── README.md                   # Este guia do projeto
│
├── codigo/                     # Módulos de código-fonte isolados por estrutura
│   ├── trie/                   # Implementação e testes da Árvore Trie
│   │   ├── trie.hpp
│   │   └── main.cpp
│   ├── patricia/               # Implementação e testes da Árvore Patricia
│   │   ├── patricia.hpp
│   │   └── main.cpp
│   ├── splay/                  # Implementação e testes da Árvore Splay
│   │   ├── splay.hpp
│   │   └── main.cpp
│   ├── treap/                  # Implementação e testes da Árvore Treap
│   │   ├── treap.hpp
│   │   └── main.cpp
│   ├── kdtree/                 # Implementação e testes da KD-Tree
│   │   ├── kdtree.hpp
│   │   └── main.cpp
│   ├── avl/                    # Baseline: Árvore AVL genérica
│   │   └── avl.hpp
│   ├── bst/                    # Baseline: Árvore Binária de Busca iterativa
│   │   └── bst.hpp
│   └── benchmark/              # Bateria empírica automatizada
│       ├── benchmark.cpp       # Harness de benchmark C++20 com 4 cenários
│       ├── gerar_graficos.py   # Gerador de gráficos em Matplotlib (300 DPI)
│       └── resultados/         # Saídas em CSV dos ensaios computacionais
│           ├── cenario1_strings.csv
│           ├── cenario2_ordenada.csv
│           ├── cenario3_localidade.csv
│           └── cenario4_kdtree.csv
│
├── imagens/                    # Gráficos de benchmark em alta resolução
│   ├── cenario1_prefixos.png
│   ├── cenario2_altura_ordenada.png
│   ├── cenario3_localidade_splay.png
│   └── cenario4_maldicao_kdtree.png
│
└── legado/                     # Arquivos do estado anterior mantidos para histórico
```

---

## Instruções de Compilação e Execução

### 1. Testes Unitários das Estruturas (C++20)

Para compilar e executar o teste individual de cada estrutura:

```bash
# Trie
g++ -std=c++20 -O2 codigo/trie/main.cpp -o codigo/trie/teste_trie
./codigo/trie/teste_trie

# Patricia
g++ -std=c++20 -O2 codigo/patricia/main.cpp -o codigo/patricia/teste_patricia
./codigo/patricia/teste_patricia

# Splay
g++ -std=c++20 -O2 codigo/splay/main.cpp -o codigo/splay/teste_splay
./codigo/splay/teste_splay

# Treap
g++ -std=c++20 -O2 codigo/treap/main.cpp -o codigo/treap/teste_treap
./codigo/treap/teste_treap

# KD-Tree
g++ -std=c++20 -O2 codigo/kdtree/main.cpp -o codigo/kdtree/teste_kdtree
./codigo/kdtree/teste_kdtree
```

### 2. Bateria de Benchmarks Empíricos

O benchmark executa os 4 cenários metodológicos:

1. **Cenário 1 (Prefixos / Strings):** Trie vs Patricia vs AVL vs BST para $N \in [10^4, 10^5]$.
2. **Cenário 2 (Inserção Ordenada Patológica):** BST vs AVL vs Treap vs Splay para $N \in [10^3, 2\times 10^4]$.
3. **Cenário 3 (Localidade Temporal e Zipf):** Splay vs AVL vs Treap sob cargas Zipf 80/20 vs Uniforme ($M=10^5$ consultas).
4. **Cenário 4 (Busca Espacial k-NN):** KD-Tree vs Busca Linear em $N=50.000$ pontos para $k \in \{2, 3, 5, 8, 10, 15, 20\}$.

Para compilar, rodar e exportar os CSVs:

```bash
g++ -std=c++20 -O3 codigo/benchmark/benchmark.cpp -o codigo/benchmark/run_benchmarks
./codigo/benchmark/run_benchmarks
```

Para gerar os gráficos em `imagens/`:

```bash
python codigo/benchmark/gerar_graficos.py
```

### 3. Compilação do Artigo em LaTeX

O relatório utiliza pacotes padrão MiKTeX/TeXLive com suporte a TikZ, Microtype e Tabularx:

```bash
pdflatex -interaction=nonstopmode -halt-on-error main.tex
pdflatex -interaction=nonstopmode -halt-on-error main.tex
```

O documento final `main.pdf` possui rigorosamente **12 páginas**, atendendo a todas as normas e seções exigidas pelo edital.

---

## Repositório Remoto
- GitHub: [https://github.com/igorpeixoto/AEDESII-Arvores](https://github.com/igorpeixoto/AEDESII-Arvores)
