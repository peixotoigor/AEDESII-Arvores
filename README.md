# Estruturas de Dados Avançadas em Árvore — Estudo Teórico e Empírico

Trabalho prático da disciplina de **Algoritmos e Estruturas de Dados II (AEDs II)** — **CEFET-MG**.  
Implementação procedural completa em **C++20** das estruturas: **Trie**, **Patricia**, **Splay**, **Treap**, **KD-Tree**, além das referências **BST** e **AVL**.

---

## ⚡ Início Rápido (Execução em 1 Comando)

Para validar dependências, compilar o motor C++20 com `-O3 -march=native`, executar as 10 repetições de benchmarks e gerar todos os resultados e gráficos:

```bash
python codigo/benchmark/executar_benchmark_completo.py
```

---

## 📋 Pré-requisitos Mínimos

- **Compilador C++20:** `g++` versão $\ge$ 13 (no Linux nativo, MinGW ou via WSL2 no Windows).
- **Python 3:** Python $\ge$ 3.10 com as bibliotecas:
  ```bash
  pip install matplotlib numpy
  ```

---

## 🧪 Como Executar os Testes Unitários (7 Estruturas)

Para testar todas as 7 implementações com asserções estritas (`assert`) consumindo os conjuntos de dados reais em `codigo/benchmark/dados/`:

```bash
python codigo/executar_todos_testes.py
```

### Execução Individual de Cada Estrutura:
```bash
# Trie / Patricia / Splay / Treap / KD-Tree / AVL / BST
g++ -std=c++20 -O2 codigo/trie/main.cpp -o teste_trie && ./teste_trie
g++ -std=c++20 -O2 codigo/patricia/main.cpp -o teste_patricia && ./teste_patricia
g++ -std=c++20 -O2 codigo/splay/main.cpp -o teste_splay && ./teste_splay
g++ -std=c++20 -O2 codigo/treap/main.cpp -o teste_treap && ./teste_treap
g++ -std=c++20 -O2 codigo/kdtree/main.cpp -o teste_kdtree && ./teste_kdtree
g++ -std=c++20 -O2 codigo/avl/main.cpp -o teste_avl && ./teste_avl
g++ -std=c++20 -O2 codigo/bst/main.cpp -o teste_bst && ./teste_bst
```

---

## 📊 Onde Estão os Resultados e as Imagens Geradas

Ao executar o benchmark, os artefatos são atualizados automaticamente nas seguintes pastas:

### 1. Dados Numéricos (Arquivos CSV)
Pasta: `codigo/benchmark/resultados/`
- `cenario1_strings.csv`: Inserção, busca, nós alocados e taxa de compressão (Trie vs Patricia vs AVL vs BST para $N \le 100.000$).
- `cenario2_ordenada.csv`: Degeneração patológica da BST ($h=50.000$) vs AVL ($h=16$), Treap ($h \le 38$) e Splay.
- `cenario3_localidade.csv`: Aceleração da Splay sob distribuição de Zipf 80/20 ($100.000$ consultas).
- `cenario4_kdtree.csv`: Busca 1-NN da KD-Tree vs Varredura Linear em $k \in \{2, 3, 5, 10, 15, 20\}$ dimensões.

### 2. Painéis Gráficos Compostos (para o Artigo)
Pasta: `imagens/`
- `cenario1_prefixos.png`: Painel com tempo de inserção, busca constante $\mathcal{O}(L)$ e compactação de 91,5% de nós da Patricia.
- `cenario2_altura_ordenada.png`: Altura logarítmica vs colapso linear e curva de tempo quadrático da BST.
- `cenario3_localidade_splay.png`: Comparativo de latência média e ganho de 29,0% da Splay sob localidade temporal.
- `cenario4_maldicao_kdtree.png`: Speedup de $225,7\times$ em 2D e colapso da poda geométrica com vitória da busca linear em $k \ge 15$.
- `kdtree_particao_2d.png`: Particionamento geométrico espacial em $\mathbb{R}^2$ com raio de busca euclidiana.

### 3. Figuras Individuais em Alta Resolução (para Auditoria e Inspeção)
Pasta: `imagens/separadas/`
- Contém os 10 gráficos isolados em arquivos PNG individuais (ex: `cenario1_a_tempo_insercao.png`, `cenario4_b_percentual_nos_visitados.png`, etc.).

---

## 📄 Como Compilar o Artigo Científico (LaTeX)

O repositório inclui o artigo acadêmico completo com 31 páginas, diagramas TikZ e tabelas de resultados. Para compilar via MiKTeX ou TeXLive:

```bash
pdflatex -interaction=nonstopmode main.tex
bibtex main
pdflatex -interaction=nonstopmode main.tex
pdflatex -interaction=nonstopmode main.tex
```
O arquivo de saída gerado é [`main.pdf`](main.pdf).

---

## 📁 Estrutura de Arquivos

```text
.
├── main.tex                    # Artigo acadêmico completo em LaTeX (31 páginas)
├── main.pdf                    # PDF compilado final do artigo
├── referencias.bib             # Catálogo BibTeX de referências seminais
├── REPRODUTIBILIDADE.md        # Documento formal com protocolo estatístico e hardware
├── README.md                   # Este guia rápido de execução
│
├── codigo/                     # Módulos em C++20 (estilo procedural, 2 arquivos por árvore)
│   ├── trie/                   # trie.hpp e main.cpp
│   ├── patricia/               # patricia.hpp e main.cpp
│   ├── splay/                  # splay.hpp e main.cpp
│   ├── treap/                  # treap.hpp e main.cpp
│   ├── kdtree/                 # kdtree.hpp e main.cpp
│   ├── avl/                    # avl.hpp e main.cpp
│   ├── bst/                    # bst.hpp e main.cpp
│   ├── executar_todos_testes.py # Suíte de testes data-driven
│   └── benchmark/              # Motor experimental
│       ├── benchmark.cpp       # Código C++ dos 4 cenários (10 repetições)
│       ├── gerar_graficos.py   # Gerador de gráficos Matplotlib
│       ├── executar_benchmark_completo.py # Script mestre em comando único
│       ├── dados/              # Datasets brutos de entrada (.txt)
│       └── resultados/         # Resultados experimentais consolidados (.csv)
│
└── imagens/                    # Painéis compostos do artigo
    └── separadas/              # 10 gráficos individuais de conferência
```

---

## 👤 Autoria
- **Autor:** Igor Peixoto Rodrigues
- **Instituição:** Centro Federal de Educação Tecnológica de Minas Gerais (CEFET-MG)
- **Disciplina:** Algoritmos e Estruturas de Dados II
