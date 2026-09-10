#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <filesystem>

#include "../trie/trie.hpp"
#include "../patricia/patricia.hpp"
#include "../splay/splay.hpp"
#include "../treap/treap.hpp"
#include "../kdtree/kdtree.hpp"
#include "../bst/bst.hpp"
#include "../avl/avl.hpp"

namespace fs = std::filesystem;

// Impede que o otimizador do compilador (-O3) descarte laços de benchmark
template <typename T>
inline void doNotOptimize(const T& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

class Cronometro {
    std::chrono::high_resolution_clock::time_point inicio;
public:
    void iniciar() {
        inicio = std::chrono::high_resolution_clock::now();
    }
    double decorridoMs() const {
        auto fim = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(fim - inicio).count();
    }
    double decorridoUs() const {
        auto fim = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::micro>(fim - inicio).count();
    }
};

// ============================================================================
// CENÁRIO 1: Prefixos e Strings (Trie vs Patricia vs AVL vs BST)
// ============================================================================
std::vector<std::string> gerarPalavras(size_t n, unsigned int semente = 42) {
    std::mt19937 rng(semente);
    std::uniform_int_distribution<int> distChar('a', 'z');
    std::uniform_int_distribution<int> distTam(4, 12);

    std::vector<std::string> prefixos;
    for (int i = 0; i < 200; ++i) {
        std::string p;
        int ptam = 3 + (i % 6);
        for (int j = 0; j < ptam; ++j) p += (char)distChar(rng);
        prefixos.push_back(p);
    }

    std::vector<std::string> palavras;
    palavras.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        std::string w = prefixos[i % prefixos.size()];
        int resto = distTam(rng);
        for (int j = 0; j < resto; ++j) w += (char)distChar(rng);
        w += std::to_string(i / prefixos.size());
        palavras.push_back(w);
    }
    return palavras;
}

void executarCenario1() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 1: Prefixos e Strings...\n";
    std::cout << "======================================================\n";

    std::ofstream arq("codigo/benchmark/resultados/cenario1_strings.csv");
    arq << "N,Estrutura,TempoInsercaoMs,TempoBuscaMs,QtdNos,TaxaCompressao\n";

    std::vector<size_t> tamanhos = {10000, 25000, 50000, 100000};
    Cronometro crono;

    for (size_t n : tamanhos) {
        std::cout << "  -> Testando N = " << n << " strings...\n";
        auto palavras = gerarPalavras(n);

        std::vector<std::string> buscas;
        for (size_t i = 0; i < 5000 && i < n; ++i) buscas.push_back(palavras[i]);
        for (size_t i = 0; i < 5000; ++i) buscas.push_back("inexistente_" + std::to_string(i));

        // 1. Trie
        {
            Trie trie;
            crono.iniciar();
            for (const auto& p : palavras) trie.inserir(p);
            double tIns = crono.decorridoMs();

            crono.iniciar();
            for (const auto& q : buscas) {
                bool r = trie.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            size_t nos = trie.contarNos();
            arq << n << ",Trie," << tIns << "," << tBus << "," << nos << ",1.000\n";
        }

        // 2. Patricia Tree
        {
            Patricia patricia;
            crono.iniciar();
            for (const auto& p : palavras) patricia.inserir(p);
            double tIns = crono.decorridoMs();

            crono.iniciar();
            for (const auto& q : buscas) {
                bool r = patricia.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            size_t nos = patricia.contarNos();
            double taxa = (double)nos / (n * 7.5);
            arq << n << ",Patricia," << tIns << "," << tBus << "," << nos << "," << taxa << "\n";
        }

        // 3. AVL
        {
            AVL<std::string> avl;
            crono.iniciar();
            for (const auto& p : palavras) avl.inserir(p);
            double tIns = crono.decorridoMs();

            crono.iniciar();
            for (const auto& q : buscas) {
                bool r = avl.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",AVL," << tIns << "," << tBus << "," << avl.tamanho() << ",N/A\n";
        }

        // 4. BST
        {
            BST<std::string> bst;
            crono.iniciar();
            for (const auto& p : palavras) bst.inserir(p);
            double tIns = crono.decorridoMs();

            crono.iniciar();
            for (const auto& q : buscas) {
                bool r = bst.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",BST," << tIns << "," << tBus << "," << bst.tamanho() << ",N/A\n";
        }
    }
    arq.close();
    std::cout << "  [OK] Cenario 1 concluido e salvo.\n";
}

// ============================================================================
// CENÁRIO 2: Inserção Ordenada Patológica (BST vs AVL vs Splay vs Treap)
// ============================================================================
void executarCenario2() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 2: Insercao Ordenada Patologica...\n";
    std::cout << "======================================================\n";

    std::ofstream arq("codigo/benchmark/resultados/cenario2_ordenada.csv");
    arq << "N,Estrutura,Altura,TempoInsercaoMs,TempoBuscaMs\n";

    std::vector<int> tamanhos = {1000, 2500, 5000, 10000, 15000, 20000};
    Cronometro crono;

    for (int n : tamanhos) {
        std::cout << "  -> Testando N = " << n << " elementos ordenados...\n";

        std::vector<int> buscas;
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(1, n);
        for (int i = 0; i < 2000; ++i) buscas.push_back(dist(rng));

        // 1. BST (Degenera para O(N))
        {
            BST<int> bst;
            crono.iniciar();
            for (int i = 1; i <= n; ++i) bst.inserir(i);
            double tIns = crono.decorridoMs();

            size_t h = bst.altura();

            crono.iniciar();
            for (int q : buscas) {
                bool r = bst.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",BST," << h << "," << tIns << "," << tBus << "\n";
        }

        // 2. AVL (Mantem balanceamento estrito)
        {
            AVL<int> avl;
            crono.iniciar();
            for (int i = 1; i <= n; ++i) avl.inserir(i);
            double tIns = crono.decorridoMs();

            size_t h = avl.altura();

            crono.iniciar();
            for (int q : buscas) {
                bool r = avl.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",AVL," << h << "," << tIns << "," << tBus << "\n";
        }

        // 3. Treap (Prioridades aleatorias impedem degeneracao)
        {
            Treap treap(42);
            crono.iniciar();
            for (int i = 1; i <= n; ++i) treap.inserir(i);
            double tIns = crono.decorridoMs();

            size_t h = treap.altura();

            crono.iniciar();
            for (int q : buscas) {
                bool r = treap.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",Treap," << h << "," << tIns << "," << tBus << "\n";
        }

        // 4. Splay Tree (Rotaciona para a raiz)
        {
            SplayTree splay;
            crono.iniciar();
            for (int i = 1; i <= n; ++i) splay.inserir(i);
            double tIns = crono.decorridoMs();

            size_t h = splay.altura();

            crono.iniciar();
            for (int q : buscas) {
                bool r = splay.buscar(q);
                doNotOptimize(r);
            }
            double tBus = crono.decorridoMs();

            arq << n << ",Splay," << h << "," << tIns << "," << tBus << "\n";
        }
    }
    arq.close();
    std::cout << "  [OK] Cenario 2 concluido e salvo.\n";
}

// ============================================================================
// CENÁRIO 3: Localidade Temporal e Acesso Zipf (Splay vs AVL vs Treap)
// ============================================================================
void executarCenario3() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 3: Localidade Temporal (Zipf)...\n";
    std::cout << "======================================================\n";

    std::ofstream arq("codigo/benchmark/resultados/cenario3_localidade.csv");
    arq << "Distribuicao,Estrutura,TempoTotalMs,LatenciaMediaUs\n";

    const int N = 50000;
    const int M = 100000;

    std::mt19937 rng(42);
    std::vector<int> chaves(N);
    for (int i = 0; i < N; ++i) chaves[i] = i * 2;
    std::shuffle(chaves.begin(), chaves.end(), rng);

    // Monta arvores
    SplayTree splay;
    AVL<int> avl;
    Treap treap(42);

    for (int k : chaves) {
        splay.inserir(k);
        avl.inserir(k);
        treap.inserir(k);
    }

    // 1. Carga com Localidade Temporal Estrita (Zipfian/Pareto: 85% dos acessos em 5% dos itens)
    std::vector<int> consultasZipf;
    consultasZipf.reserve(M);
    std::uniform_real_distribution<double> distU(0.0, 1.0);
    for (int i = 0; i < M; ++i) {
        double u = distU(rng);
        int idx = static_cast<int>(N * std::pow(u, 3.5));
        if (idx >= N) idx = N - 1;
        consultasZipf.push_back(chaves[idx]);
    }

    // 2. Carga Uniforme (Sem localidade)
    std::vector<int> consultasUniforme;
    consultasUniforme.reserve(M);
    std::uniform_int_distribution<int> distUni(0, N - 1);
    for (int i = 0; i < M; ++i) {
        consultasUniforme.push_back(chaves[distUni(rng)]);
    }

    Cronometro crono;

    auto testar = [&](const std::string& distNome, const std::vector<int>& queries) {
        // Splay
        crono.iniciar();
        for (int q : queries) {
            bool r = splay.buscar(q);
            doNotOptimize(r);
        }
        double tSplay = crono.decorridoMs();
        arq << distNome << ",Splay," << tSplay << "," << (tSplay * 1000.0 / M) << "\n";

        // AVL
        crono.iniciar();
        for (int q : queries) {
            bool r = avl.buscar(q);
            doNotOptimize(r);
        }
        double tAVL = crono.decorridoMs();
        arq << distNome << ",AVL," << tAVL << "," << (tAVL * 1000.0 / M) << "\n";

        // Treap
        crono.iniciar();
        for (int q : queries) {
            bool r = treap.buscar(q);
            doNotOptimize(r);
        }
        double tTreap = crono.decorridoMs();
        arq << distNome << ",Treap," << tTreap << "," << (tTreap * 1000.0 / M) << "\n";
    };

    std::cout << "  -> Testando carga com forte localidade temporal (Zipf)...\n";
    testar("Zipf (Localidade)", consultasZipf);

    std::cout << "  -> Testando carga Uniforme...\n";
    testar("Uniforme", consultasUniforme);

    arq.close();
    std::cout << "  [OK] Cenario 3 concluido e salvo.\n";
}

// ============================================================================
// CENÁRIO 4: Busca k-NN e a Maldição da Dimensionalidade (KD-Tree vs Linear)
// ============================================================================
void executarCenario4() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 4: Maldicao da Dimensionalidade...\n";
    std::cout << "======================================================\n";

    std::ofstream arq("codigo/benchmark/resultados/cenario4_kdtree.csv");
    arq << "Dimensao,TempoKDTreeUs,TempoLinearUs,NosVisitadosPerc,Speedup\n";

    const size_t N = 50000;
    const size_t Q = 200;
    std::vector<size_t> dimensoes = {2, 3, 5, 8, 10, 15, 20};

    Cronometro crono;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> distCoord(0.0, 1.0);

    for (size_t k : dimensoes) {
        std::cout << "  -> Testando Dimensao k = " << k << " com " << N << " pontos...\n";

        std::vector<KDTree::Ponto> pontos(N, KDTree::Ponto(k));
        for (size_t i = 0; i < N; ++i) {
            for (size_t d = 0; d < k; ++d) {
                pontos[i][d] = distCoord(rng);
            }
        }

        std::vector<KDTree::Ponto> queries(Q, KDTree::Ponto(k));
        for (size_t i = 0; i < Q; ++i) {
            for (size_t d = 0; d < k; ++d) {
                queries[i][d] = distCoord(rng);
            }
        }

        KDTree kdt(k);
        kdt.construirBalanceada(pontos);

        // Busca 1-NN com KD-Tree
        size_t totalVisitados = 0;
        crono.iniciar();
        for (const auto& q : queries) {
            size_t vis = 0;
            auto pt = kdt.vizinhoMaisProximo(q, &vis);
            doNotOptimize(pt[0]);
            totalVisitados += vis;
        }
        double tKDTreeUs = crono.decorridoUs() / Q;
        double percVisitados = (double)totalVisitados / (Q * N) * 100.0;

        // Busca 1-NN com Força Bruta (Linear Scan)
        crono.iniciar();
        for (const auto& q : queries) {
            double melhorDist = std::numeric_limits<double>::infinity();
            size_t melhorIdx = 0;
            for (size_t i = 0; i < N; ++i) {
                double d = 0.0;
                for (size_t dim = 0; dim < k; ++dim) {
                    double diff = q[dim] - pontos[i][dim];
                    d += diff * diff;
                }
                if (d < melhorDist) {
                    melhorDist = d;
                    melhorIdx = i;
                }
            }
            doNotOptimize(melhorDist);
            doNotOptimize(melhorIdx);
        }
        double tLinearUs = crono.decorridoUs() / Q;
        double speedup = tLinearUs / tKDTreeUs;

        arq << k << "," << tKDTreeUs << "," << tLinearUs << "," << percVisitados << "," << speedup << "\n";
    }

    arq.close();
    std::cout << "  [OK] Cenario 4 concluido e salvo.\n";
}

// ============================================================================
// MAIN: Orquestrador Geral de Benchmarks
// ============================================================================
int main() {
    std::cout << "===============================================================\n";
    std::cout << "   AEDES II - BATERIA DE BENCHMARKS EMPIRICOS (C++20)\n";
    std::cout << "===============================================================\n";

    fs::create_directories("codigo/benchmark/resultados");

    executarCenario1();
    executarCenario2();
    executarCenario3();
    executarCenario4();

    std::cout << "\n===============================================================\n";
    std::cout << "   [SUCESSO] Todos os cenarios concluidos com exito!\n";
    std::cout << "   Arquivos CSV gerados em: codigo/benchmark/resultados/\n";
    std::cout << "===============================================================\n";

    return 0;
}
