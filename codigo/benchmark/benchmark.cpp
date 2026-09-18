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
#include <numeric>
#include <sstream>

#include "../trie/trie.hpp"
#include "../patricia/patricia.hpp"
#include "../splay/splay.hpp"
#include "../treap/treap.hpp"
#include "../kdtree/kdtree.hpp"
#include "../bst/bst.hpp"
#include "../avl/avl.hpp"

namespace fs = std::filesystem;

// Bateria de testes de desempenho para comparar as arvores

void doNotOptimize(bool val) {
    asm volatile("" : : "r,m"(val) : "memory");
}
void doNotOptimize(int val) {
    asm volatile("" : : "r,m"(val) : "memory");
}
void doNotOptimize(double val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

// Estruturas auxiliares simples para comparacao de strings no Cenario 1
struct NoBST_Str {
    std::string chave;
    NoBST_Str* esq = nullptr;
    NoBST_Str* dir = nullptr;
};

NoBST_Str* inserirBST_Str(NoBST_Str* raiz, const std::string& chave) {
    if (raiz == nullptr) return new NoBST_Str{chave, nullptr, nullptr};
    NoBST_Str* atual = raiz;
    while (true) {
        if (chave < atual->chave) {
            if (atual->esq == nullptr) { atual->esq = new NoBST_Str{chave, nullptr, nullptr}; break; }
            atual = atual->esq;
        } else if (chave > atual->chave) {
            if (atual->dir == nullptr) { atual->dir = new NoBST_Str{chave, nullptr, nullptr}; break; }
            atual = atual->dir;
        } else break;
    }
    return raiz;
}

bool buscarBST_Str(NoBST_Str* raiz, const std::string& chave) {
    NoBST_Str* atual = raiz;
    while (atual != nullptr) {
        if (chave < atual->chave) atual = atual->esq;
        else if (chave > atual->chave) atual = atual->dir;
        else return true;
    }
    return false;
}

void liberarBST_Str(NoBST_Str* raiz) {
    if (raiz != nullptr) {
        liberarBST_Str(raiz->esq);
        liberarBST_Str(raiz->dir);
        delete raiz;
    }
}

struct NoAVL_Str {
    std::string chave;
    int altura = 1;
    NoAVL_Str* esq = nullptr;
    NoAVL_Str* dir = nullptr;
};

int obterAlturaAVL_Str(NoAVL_Str* no) {
    return (no == nullptr) ? 0 : no->altura;
}

int fatorBalancoAVL_Str(NoAVL_Str* no) {
    return (no == nullptr) ? 0 : (obterAlturaAVL_Str(no->dir) - obterAlturaAVL_Str(no->esq));
}

void atualizarAlturaAVL_Str(NoAVL_Str* no) {
    if (no != nullptr) {
        no->altura = 1 + std::max(obterAlturaAVL_Str(no->esq), obterAlturaAVL_Str(no->dir));
    }
}

NoAVL_Str* rotacionarDireitaAVL_Str(NoAVL_Str* y) {
    NoAVL_Str* x = y->esq;
    NoAVL_Str* t2 = x->dir;
    x->dir = y;
    y->esq = t2;
    atualizarAlturaAVL_Str(y);
    atualizarAlturaAVL_Str(x);
    return x;
}

NoAVL_Str* rotacionarEsquerdaAVL_Str(NoAVL_Str* x) {
    NoAVL_Str* y = x->dir;
    NoAVL_Str* t2 = y->esq;
    y->esq = x;
    x->dir = t2;
    atualizarAlturaAVL_Str(x);
    atualizarAlturaAVL_Str(y);
    return y;
}

NoAVL_Str* inserirAVL_Str(NoAVL_Str* no, const std::string& chave) {
    if (no == nullptr) return new NoAVL_Str{chave, 1, nullptr, nullptr};
    if (chave < no->chave) no->esq = inserirAVL_Str(no->esq, chave);
    else if (chave > no->chave) no->dir = inserirAVL_Str(no->dir, chave);
    else return no;

    atualizarAlturaAVL_Str(no);
    int fb = fatorBalancoAVL_Str(no);

    if (fb < -1 && chave < no->esq->chave) return rotacionarDireitaAVL_Str(no);
    if (fb > 1 && chave > no->dir->chave) return rotacionarEsquerdaAVL_Str(no);
    if (fb < -1 && chave > no->esq->chave) {
        no->esq = rotacionarEsquerdaAVL_Str(no->esq);
        return rotacionarDireitaAVL_Str(no);
    }
    if (fb > 1 && chave < no->dir->chave) {
        no->dir = rotacionarDireitaAVL_Str(no->dir);
        return rotacionarEsquerdaAVL_Str(no);
    }
    return no;
}

bool buscarAVL_Str(NoAVL_Str* raiz, const std::string& chave) {
    NoAVL_Str* atual = raiz;
    while (atual != nullptr) {
        if (chave < atual->chave) atual = atual->esq;
        else if (chave > atual->chave) atual = atual->dir;
        else return true;
    }
    return false;
}

void liberarAVL_Str(NoAVL_Str* raiz) {
    if (raiz != nullptr) {
        liberarAVL_Str(raiz->esq);
        liberarAVL_Str(raiz->dir);
        delete raiz;
    }
}


struct Cronometro {
    std::chrono::high_resolution_clock::time_point inicio;

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

struct Estatisticas {
    double media = 0.0;
    double desvioPadrao = 0.0;
};

Estatisticas calcularEstatisticas(const std::vector<double>& amostras) {
    if (amostras.empty()) return {0.0, 0.0};
    double soma = std::accumulate(amostras.begin(), amostras.end(), 0.0);
    double m = soma / amostras.size();
    if (amostras.size() <= 1) return {m, 0.0};

    double somaVar = 0.0;
    for (double x : amostras) {
        somaVar += (x - m) * (x - m);
    }
    double s = std::sqrt(somaVar / (amostras.size() - 1));
    return {m, s};
}

constexpr int NUM_REPETICOES = 10;

// Funções auxiliares para resolução flexível de caminhos
std::string resolverCaminho(const std::string& caminho) {
    if (fs::exists(caminho)) return caminho;
    if (caminho.rfind("codigo/benchmark/", 0) == 0) {
        std::string sub = caminho.substr(17);
        if (fs::exists(sub)) return sub;
    }
    return caminho;
}

std::string resolverCaminhoSaida(const std::string& nomeArquivo) {
    if (fs::exists("codigo/benchmark/resultados") || fs::exists("codigo/benchmark")) {
        fs::create_directories("codigo/benchmark/resultados");
        return "codigo/benchmark/resultados/" + nomeArquivo;
    }
    fs::create_directories("resultados");
    return "resultados/" + nomeArquivo;
}

// Função auxiliar para carregar linhas de arquivo de texto limpando '\r'
std::vector<std::string> carregarLinhas(const std::string& caminho, size_t limite = 0) {
    std::vector<std::string> linhas;
    std::string caminhoReal = resolverCaminho(caminho);
    std::ifstream arq(caminhoReal);
    if (!arq.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir arquivo de dados: " << caminhoReal << std::endl;
        return linhas;
    }
    std::string linha;
    while (std::getline(arq, linha)) {
        while (!linha.empty() && (linha.back() == '\r' || linha.back() == ' ' || linha.back() == '\t')) {
            linha.pop_back();
        }
        if (linha.empty() || linha[0] == '#') continue;
        linhas.push_back(linha);
        if (limite > 0 && linhas.size() >= limite) break;
    }
    return linhas;
}

// ============================================================================
// CENÁRIO 1: Prefixos e Strings (Trie vs Patricia vs AVL vs BST)
// main.tex §6.1: 2 datasets (vocabulario real + prefixos extensos L in [20, 100])
// N in [10^3, 10^5] palavras (1.000, 10.000, 50.000, 100.000)
// ============================================================================
void executarCenario1() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 1: Prefixos e Strings (" << NUM_REPETICOES << " repeticoes)...\n";
    std::cout << "======================================================\n";

    std::string caminhoCsv = resolverCaminhoSaida("cenario1_strings.csv");
    std::ofstream arq(caminhoCsv);
    arq << "N,Estrutura,Dataset,TempoInsercaoMs,TempoInsercaoStd,TempoBuscaMs,TempoBuscaStd,QtdNos,TaxaCompressao\n";

    std::vector<size_t> tamanhos = {1000, 10000, 50000, 100000};
    Cronometro crono;

    struct ConfigDataset {
        std::string nome;
        std::string arquivo;
    };

    std::vector<ConfigDataset> datasets = {
        {"VocabularioReal", "codigo/benchmark/dados/cenario1_vocabulario_real.txt"},
        {"PrefixosExtensos", "codigo/benchmark/dados/cenario1_prefixos_extensos.txt"}
    };

    for (const auto& ds : datasets) {
        std::cout << "  [DATASET] Carregando: " << ds.nome << " (" << ds.arquivo << ")..." << std::endl;
        auto todasPalavras = carregarLinhas(ds.arquivo, 100000);
        if (todasPalavras.empty()) {
            std::cerr << "  [ERRO] Dataset vazio ou inexistente: " << ds.arquivo << std::endl;
            continue;
        }

        for (size_t n : tamanhos) {
            std::cout << "    -> Testando N = " << n << " strings (" << ds.nome << ")..." << std::endl;

            std::vector<double> tInsTrie, tBusTrie;
            std::vector<double> tInsPat,  tBusPat;
            std::vector<double> tInsAVL,  tBusAVL;
            std::vector<double> tInsBST,  tBusBST;
            size_t nosTrie = 0, nosPat = 0;

            for (int rep = 0; rep < NUM_REPETICOES; ++rep) {
                // Seleciona as primeiras n palavras para consistência
                std::vector<std::string> palavras(todasPalavras.begin(), todasPalavras.begin() + std::min(n, todasPalavras.size()));

                // Prepara 5.000 buscas (2.500 existentes + 2.500 inexistentes)
                std::vector<std::string> buscas;
                size_t qtdExistentes = std::min((size_t)2500, n);
                for (size_t i = 0; i < qtdExistentes; ++i) {
                    buscas.push_back(palavras[(i * 37) % n]);
                }
                for (size_t i = 0; i < 2500; ++i) {
                    buscas.push_back("inexistente_termo_benchmark_" + std::to_string(i));
                }

                // 1. Trie
                {
                    NoTrie* trie = criarNoTrie();
                    crono.iniciar();
                    for (const auto& p : palavras) inserirTrie(trie, p);
                    tInsTrie.push_back(crono.decorridoMs());

                    crono.iniciar();
                    for (const auto& q : buscas) {
                        bool r = buscarTrie(trie, q);
                        doNotOptimize(r);
                    }
                    tBusTrie.push_back(crono.decorridoMs());
                    if (rep == 0) nosTrie = contarNosTrie(trie);
                    liberarTrie(trie);
                }

                // 2. Patricia
                {
                    NoPatricia* patricia = criarNoPatricia(false);
                    crono.iniciar();
                    for (const auto& p : palavras) inserirPatricia(patricia, p);
                    tInsPat.push_back(crono.decorridoMs());

                    crono.iniciar();
                    for (const auto& q : buscas) {
                        bool r = buscarPatricia(patricia, q);
                        doNotOptimize(r);
                    }
                    tBusPat.push_back(crono.decorridoMs());
                    if (rep == 0) nosPat = contarNosPatricia(patricia);
                    liberarPatricia(patricia);
                }

                // 3. AVL
                {
                    NoAVL_Str* avl = nullptr;
                    crono.iniciar();
                    for (const auto& p : palavras) avl = inserirAVL_Str(avl, p);
                    tInsAVL.push_back(crono.decorridoMs());

                    crono.iniciar();
                    for (const auto& q : buscas) {
                        bool r = buscarAVL_Str(avl, q);
                        doNotOptimize(r);
                    }
                    tBusAVL.push_back(crono.decorridoMs());
                    liberarAVL_Str(avl);
                }

                // 4. BST
                {
                    NoBST_Str* bst = nullptr;
                    crono.iniciar();
                    for (const auto& p : palavras) bst = inserirBST_Str(bst, p);
                    tInsBST.push_back(crono.decorridoMs());

                    crono.iniciar();
                    for (const auto& q : buscas) {
                        bool r = buscarBST_Str(bst, q);
                        doNotOptimize(r);
                    }
                    tBusBST.push_back(crono.decorridoMs());
                    liberarBST_Str(bst);
                }
            }

            auto statInsTrie = calcularEstatisticas(tInsTrie);
            auto statBusTrie = calcularEstatisticas(tBusTrie);
            auto statInsPat  = calcularEstatisticas(tInsPat);
            auto statBusPat  = calcularEstatisticas(tBusPat);
            auto statInsAVL  = calcularEstatisticas(tInsAVL);
            auto statBusAVL  = calcularEstatisticas(tBusAVL);
            auto statInsBST  = calcularEstatisticas(tInsBST);
            auto statBusBST  = calcularEstatisticas(tBusBST);

            double taxaPat = (nosTrie > 0) ? ((double)nosPat / nosTrie) : 1.0;

            arq << n << ",Trie," << ds.nome << "," << statInsTrie.media << "," << statInsTrie.desvioPadrao 
                << "," << statBusTrie.media << "," << statBusTrie.desvioPadrao << "," << nosTrie << ",1.000\n";
            arq << n << ",Patricia," << ds.nome << "," << statInsPat.media << "," << statInsPat.desvioPadrao 
                << "," << statBusPat.media << "," << statBusPat.desvioPadrao << "," << nosPat << "," << taxaPat << "\n";
            arq << n << ",AVL," << ds.nome << "," << statInsAVL.media << "," << statInsAVL.desvioPadrao 
                << "," << statBusAVL.media << "," << statBusAVL.desvioPadrao << "," << n << ",N/A\n";
            arq << n << ",BST," << ds.nome << "," << statInsBST.media << "," << statInsBST.desvioPadrao 
                << "," << statBusBST.media << "," << statBusBST.desvioPadrao << "," << n << ",N/A\n";
            arq.flush();
        }
    }
    arq.close();
    std::cout << "  [OK] Cenario 1 concluido e salvo com estatisticas." << std::endl;
}

// ============================================================================
// CENÁRIO 2: Inserção Ordenada Patológica (BST vs AVL vs Splay vs Treap)
// main.tex §6.1: n in [10^3, 5*10^4] (1.000, 5.000, 10.000, 20.000, 50.000)
// ============================================================================
void executarCenario2() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 2: Insercao Ordenada Patologica (" << NUM_REPETICOES << " repeticoes)...\n";
    std::cout << "======================================================\n";

    std::string caminhoCsv = resolverCaminhoSaida("cenario2_ordenada.csv");
    std::ofstream arq(caminhoCsv);
    arq << "N,Estrutura,Altura,AlturaStd,TempoInsercaoMs,TempoInsercaoStd,TempoBuscaMs,TempoBuscaStd\n";

    std::vector<int> tamanhos = {1000, 5000, 10000, 20000, 50000};
    Cronometro crono;

    for (int n : tamanhos) {
        std::cout << "  -> Testando N = " << n << " elementos ordenados...\n";

        std::vector<double> tInsBST, tBusBST, hBST;
        std::vector<double> tInsAVL, tBusAVL, hAVL;
        std::vector<double> tInsTreap, tBusTreap, hTreap;
        std::vector<double> tInsSplay, tBusSplay, hSplay;

        for (int rep = 0; rep < NUM_REPETICOES; ++rep) {
            std::vector<int> buscas;
            std::mt19937 rng(42 + rep * 77);
            std::uniform_int_distribution<int> dist(1, n);
            for (int i = 0; i < 2000; ++i) buscas.push_back(dist(rng));

            // 1. BST (Degenera em lista encadeada O(N))
            {
                NoBST* bst = nullptr;
                crono.iniciar();
                for (int i = 1; i <= n; ++i) bst = inserirBST(bst, i);
                tInsBST.push_back(crono.decorridoMs());
                hBST.push_back((double)alturaBST(bst));

                crono.iniciar();
                for (int q : buscas) {
                    bool r = buscarBST(bst, q);
                    doNotOptimize(r);
                }
                tBusBST.push_back(crono.decorridoMs());
                liberarBST(bst);
            }

            // 2. AVL (Auto-balanceamento estrito)
            {
                NoAVL* avl = nullptr;
                crono.iniciar();
                for (int i = 1; i <= n; ++i) avl = inserirAVL(avl, i);
                tInsAVL.push_back(crono.decorridoMs());
                hAVL.push_back((double)obterAlturaAVL(avl));

                crono.iniciar();
                for (int q : buscas) {
                    bool r = buscarAVL(avl, q);
                    doNotOptimize(r);
                }
                tBusAVL.push_back(crono.decorridoMs());
                liberarAVL(avl);
            }

            // 3. Treap (Estocástica imune a chaves ordenadas)
            {
                NoTreap* treap = nullptr;
                std::mt19937 rngTreap(42 + rep * 13);
                std::uniform_int_distribution<int> distP(1, 1000000000);
                crono.iniciar();
                for (int i = 1; i <= n; ++i) treap = inserirTreap(treap, i, distP(rngTreap));
                tInsTreap.push_back(crono.decorridoMs());
                hTreap.push_back((double)alturaTreap(treap));

                crono.iniciar();
                for (int q : buscas) {
                    bool r = buscarTreap(treap, q);
                    doNotOptimize(r);
                }
                tBusTreap.push_back(crono.decorridoMs());
                liberarTreap(treap);
            }

            // 4. Splay (Cadeia linear na inserção pura, reestruturada na busca)
            {
                NoSplay* splay = nullptr;
                crono.iniciar();
                for (int i = 1; i <= n; ++i) inserirSplay(splay, i);
                tInsSplay.push_back(crono.decorridoMs());
                hSplay.push_back((double)alturaSplay(splay));

                crono.iniciar();
                for (int q : buscas) {
                    bool r = buscarSplay(splay, q);
                    doNotOptimize(r);
                }
                tBusSplay.push_back(crono.decorridoMs());
                liberarSplay(splay);
            }
        }

        auto sInsBST = calcularEstatisticas(tInsBST);
        auto sBusBST = calcularEstatisticas(tBusBST);
        auto sHBST   = calcularEstatisticas(hBST);

        auto sInsAVL = calcularEstatisticas(tInsAVL);
        auto sBusAVL = calcularEstatisticas(tBusAVL);
        auto sHAVL   = calcularEstatisticas(hAVL);

        auto sInsTreap = calcularEstatisticas(tInsTreap);
        auto sBusTreap = calcularEstatisticas(tBusTreap);
        auto sHTreap   = calcularEstatisticas(hTreap);

        auto sInsSplay = calcularEstatisticas(tInsSplay);
        auto sBusSplay = calcularEstatisticas(tBusSplay);
        auto sHSplay   = calcularEstatisticas(hSplay);

        arq << n << ",BST,"   << sHBST.media   << "," << sHBST.desvioPadrao   << "," << sInsBST.media   << "," << sInsBST.desvioPadrao   << "," << sBusBST.media   << "," << sBusBST.desvioPadrao << "\n";
        arq << n << ",AVL,"   << sHAVL.media   << "," << sHAVL.desvioPadrao   << "," << sInsAVL.media   << "," << sInsAVL.desvioPadrao   << "," << sBusAVL.media   << "," << sBusAVL.desvioPadrao << "\n";
        arq << n << ",Treap," << sHTreap.media << "," << sHTreap.desvioPadrao << "," << sInsTreap.media << "," << sInsTreap.desvioPadrao << "," << sBusTreap.media << "," << sBusTreap.desvioPadrao << "\n";
        arq << n << ",Splay," << sHSplay.media << "," << sHSplay.desvioPadrao << "," << sInsSplay.media << "," << sInsSplay.desvioPadrao << "," << sBusSplay.media << "," << sBusSplay.desvioPadrao << "\n";
    }
    arq.close();
    std::cout << "  [OK] Cenario 2 concluido e salvo com estatisticas.\n";
}

// ============================================================================
// CENÁRIO 3: Localidade Temporal e Acesso Zipf 80/20 (Splay vs AVL vs Treap)
// main.tex §6.1: N=50.000 chaves, M=100.000 buscas sob Zipf (80% em 20%) vs Uniforme
// ============================================================================
void executarCenario3() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 3: Localidade Temporal (Zipf 80/20) (" << NUM_REPETICOES << " repeticoes)...\n";
    std::cout << "======================================================\n";

    std::string caminhoCsv = resolverCaminhoSaida("cenario3_localidade.csv");
    std::ofstream arq(caminhoCsv);
    arq << "Distribuicao,Estrutura,TempoTotalMs,TempoTotalStd,LatenciaMediaUs,LatenciaMediaStd\n";

    const int N = 50000;
    const int M = 100000;

    // Carrega os traços pré-gerados do disco para máxima fidelidade
    auto consultasZipfStr = carregarLinhas("codigo/benchmark/dados/cenario3_consultas_zipf_80_20.txt", M);
    auto consultasUniStr  = carregarLinhas("codigo/benchmark/dados/cenario3_consultas_uniforme.txt", M);

    std::vector<int> consultasZipf, consultasUniforme;
    consultasZipf.reserve(consultasZipfStr.size());
    for (const auto& s : consultasZipfStr) consultasZipf.push_back(std::stoi(s));

    consultasUniforme.reserve(consultasUniStr.size());
    for (const auto& s : consultasUniStr) consultasUniforme.push_back(std::stoi(s));

    std::cout << "  [DADOS CARREGADOS] Zipf 80/20: " << consultasZipf.size() 
              << " consultas | Uniforme: " << consultasUniforme.size() << " consultas\n";

    std::vector<double> tSplayZipf, tAVLZipf, tTreapZipf;
    std::vector<double> tSplayUni,  tAVLUni,  tTreapUni;

    for (int rep = 0; rep < NUM_REPETICOES; ++rep) {
        std::mt19937 rng(42 + rep * 99);
        std::vector<int> chaves(N);
        for (int i = 0; i < N; ++i) chaves[i] = i * 2;
        std::shuffle(chaves.begin(), chaves.end(), rng);

        NoSplay* splay = nullptr;
        NoAVL* avl = nullptr;
        NoTreap* treap = nullptr;
        std::uniform_int_distribution<int> distP(1, 1000000000);

        for (int k : chaves) {
            inserirSplay(splay, k);
            avl = inserirAVL(avl, k);
            treap = inserirTreap(treap, k, distP(rng));
        }

        Cronometro crono;

        // Zipf Splay
        crono.iniciar();
        for (int q : consultasZipf) { bool r = buscarSplay(splay, q); doNotOptimize(r); }
        tSplayZipf.push_back(crono.decorridoMs());

        // Zipf AVL
        crono.iniciar();
        for (int q : consultasZipf) { bool r = buscarAVL(avl, q); doNotOptimize(r); }
        tAVLZipf.push_back(crono.decorridoMs());

        // Zipf Treap
        crono.iniciar();
        for (int q : consultasZipf) { bool r = buscarTreap(treap, q); doNotOptimize(r); }
        tTreapZipf.push_back(crono.decorridoMs());

        // Uniforme Splay
        crono.iniciar();
        for (int q : consultasUniforme) { bool r = buscarSplay(splay, q); doNotOptimize(r); }
        tSplayUni.push_back(crono.decorridoMs());

        // Uniforme AVL
        crono.iniciar();
        for (int q : consultasUniforme) { bool r = buscarAVL(avl, q); doNotOptimize(r); }
        tAVLUni.push_back(crono.decorridoMs());

        // Uniforme Treap
        crono.iniciar();
        for (int q : consultasUniforme) { bool r = buscarTreap(treap, q); doNotOptimize(r); }
        tTreapUni.push_back(crono.decorridoMs());

        liberarSplay(splay);
        liberarAVL(avl);
        liberarTreap(treap);
    }

    auto gravar = [&](const std::string& distNome, const std::string& est, const std::vector<double>& dados) {
        auto stat = calcularEstatisticas(dados);
        double latMedia = (stat.media * 1000.0) / M;
        double latStd   = (stat.desvioPadrao * 1000.0) / M;
        arq << distNome << "," << est << "," << stat.media << "," << stat.desvioPadrao << ","
            << latMedia << "," << latStd << "\n";
    };

    gravar("Zipf (Localidade)", "Splay", tSplayZipf);
    gravar("Zipf (Localidade)", "AVL",   tAVLZipf);
    gravar("Zipf (Localidade)", "Treap", tTreapZipf);

    gravar("Uniforme", "Splay", tSplayUni);
    gravar("Uniforme", "AVL",   tAVLUni);
    gravar("Uniforme", "Treap", tTreapUni);

    arq.close();
    std::cout << "  [OK] Cenario 3 concluido e salvo com estatisticas.\n";
}

// ============================================================================
// CENÁRIO 4: Busca k-NN e a Maldição da Dimensionalidade (KD-Tree vs Linear)
// main.tex §6.1: N=100.000 pontos (10^5), k in {2, 3, 5, 10, 15, 20}
// ============================================================================
void executarCenario4() {
    std::cout << "\n======================================================\n";
    std::cout << ">>> Executando Cenario 4: Maldicao da Dimensionalidade (N=100.000, " << NUM_REPETICOES << " repeticoes)...\n";
    std::cout << "======================================================\n";

    std::string caminhoCsv = resolverCaminhoSaida("cenario4_kdtree.csv");
    std::ofstream arq(caminhoCsv);
    arq << "Dimensao,TempoKDTreeUs,TempoKDTreeStd,TempoLinearUs,TempoLinearStd,NosVisitadosPerc,NosVisitadosStd,Speedup\n";

    const size_t N = 100000;
    const size_t Q = 200;
    std::vector<size_t> dimensoes = {2, 3, 5, 10, 15, 20};

    Cronometro crono;

    for (size_t k : dimensoes) {
        std::cout << "  -> Testando Dimensao k = " << k << " com " << N << " pontos...\n";

        std::vector<double> tKDT, tLin, percVis;

        for (int rep = 0; rep < NUM_REPETICOES; ++rep) {
            std::mt19937 rng(42 + rep * 123);
            std::uniform_real_distribution<double> distCoord(0.0, 1.0);

            std::vector<PontoKD> pontos(N, PontoKD(k));
            for (size_t i = 0; i < N; ++i) {
                for (size_t d = 0; d < k; ++d) {
                    pontos[i][d] = distCoord(rng);
                }
            }

            std::vector<PontoKD> queries(Q, PontoKD(k));
            for (size_t i = 0; i < Q; ++i) {
                for (size_t d = 0; d < k; ++d) {
                    queries[i][d] = distCoord(rng);
                }
            }

            NoKD* kdt = construirBalanceadaKD(pontos, 0, pontos.size(), 0);

            // Busca 1-NN com KD-Tree
            size_t totalVisitados = 0;
            crono.iniciar();
            for (const auto& q : queries) {
                size_t vis = 0;
                auto pt = vizinhoMaisProximoKD(kdt, q, &vis);
                doNotOptimize(pt[0]);
                totalVisitados += vis;
            }
            tKDT.push_back(crono.decorridoUs() / Q);
            percVis.push_back((double)totalVisitados / (Q * N) * 100.0);
            liberarKD(kdt);

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
            tLin.push_back(crono.decorridoUs() / Q);
        }

        auto statKDT = calcularEstatisticas(tKDT);
        auto statLin = calcularEstatisticas(tLin);
        auto statVis = calcularEstatisticas(percVis);
        double speedup = (statKDT.media > 0.0) ? (statLin.media / statKDT.media) : 1.0;

        arq << k << "," << statKDT.media << "," << statKDT.desvioPadrao << ","
            << statLin.media << "," << statLin.desvioPadrao << ","
            << statVis.media << "," << statVis.desvioPadrao << "," << speedup << "\n";
    }
    arq.close();
    std::cout << "  [OK] Cenario 4 concluido e salvo com estatisticas.\n";
}

int main() {
    std::cout << "========================================================================\n";
    std::cout << "  AEDs II - Bateria de Benchmarks das Arvores\n";
    std::cout << "  Executando testes com repeticoes\n";
    std::cout << "========================================================================\n";

    executarCenario1();
    executarCenario2();
    executarCenario3();
    executarCenario4();

    std::cout << "\n========================================================================\n";
    std::cout << "  [SUCESSO] Todos os 4 cenarios experimentais foram concluidos!\n";
    std::cout << "  Arquivos CSV gravados em: codigo/benchmark/resultados/\n";
    std::cout << "========================================================================\n";

    return 0;
}
