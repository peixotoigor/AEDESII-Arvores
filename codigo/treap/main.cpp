#include "treap.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <random>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore Treap (Tree + Heap)              " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string caminho = "codigo/benchmark/dados/cenario2_sequencia_ordenada.txt";
    if (argc > 1) {
        caminho = argv[1];
    } else {
        std::ifstream teste(caminho);
        if (!teste.is_open()) {
            caminho = "../benchmark/dados/cenario2_sequencia_ordenada.txt";
        }
    }

    std::cout << "[INFO] Lendo dados fisicos de: " << caminho << std::endl;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir o arquivo: " << caminho << std::endl;
        return 1;
    }

    NoTreap* raiz = nullptr;
    int valor;
    int qtdLidos = 0;
    const int LIMITE_TESTE = 2000; // Valida os primeiros 2000 inteiros ordenados

    // Gerador de prioridades pseudo-aleatorias uniforme deterministico (semente fixa 42)
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> distPrio(1, 1000000);

    while (arq >> valor && qtdLidos < LIMITE_TESTE) {
        int prio = distPrio(rng);
        raiz = inserirTreap(raiz, valor, prio);
        qtdLidos++;
    }
    arq.close();

    std::cout << "[DADOS] Total de chaves inseridas: " << qtdLidos << std::endl;
    size_t totalNos = contarNosTreap(raiz);
    int h = alturaTreap(raiz);
    std::cout << "[ESTRUTURA] Total de nos contabilizados: " << totalNos << std::endl;
    std::cout << "[ESTRUTURA] Altura da Treap com chaves ordenadas: " << h << " (esperado O(log N) ~ 15 a 30 vs 2000 da BST)" << std::endl;

    assert(totalNos == (size_t)qtdLidos && "Todos os nos devem estar presentes.");
    assert(h <= 35 && "A Treap com prioridades aleatorias deve mitigar a degeneracao linear.");

    std::cout << "[INVARIANTES] Verificando invariantes simultaneas (BST nas chaves + Min-Heap nas prioridades)..." << std::endl;
    bool invValidas = verificarInvariantesTreap(raiz);
    std::cout << "  -> Invariantes: " << (invValidas ? "VALIDAS [OK]" : "INVALIDAS [FALHA]") << std::endl;
    assert(invValidas && "Propriedade dual de BST e Min-Heap deve ser estritamente satisfeita.");

    std::cout << "[BUSCA] Testando buscas pontuais:" << std::endl;
    assert(buscarTreap(raiz, 1) && "Chave 1 deveria existir.");
    assert(buscarTreap(raiz, qtdLidos / 2) && "Chave intermediaria deveria existir.");
    assert(buscarTreap(raiz, qtdLidos) && "Ultima chave deveria existir.");
    assert(!buscarTreap(raiz, 999999) && "Chave 999999 nao deveria existir.");
    std::cout << "  -> Chave 1: Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << (qtdLidos / 2) << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << qtdLidos << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave 999999: Nao encontrada [OK]" << std::endl;

    std::cout << "[REMOCAO] Testando remocao de elementos:" << std::endl;
    int chaveRemover1 = 1;
    int chaveRemover2 = qtdLidos / 2;
    bool rem1 = false, rem2 = false;
    raiz = removerTreap(raiz, chaveRemover1, &rem1);
    raiz = removerTreap(raiz, chaveRemover2, &rem2);
    assert(rem1 && "Chave 1 deveria ser removida com sucesso.");
    assert(rem2 && "Chave mediana deveria ser removida com sucesso.");
    assert(!buscarTreap(raiz, chaveRemover1) && "Chave removida nao deve ser encontrada.");
    assert(!buscarTreap(raiz, chaveRemover2) && "Chave removida nao deve ser encontrada.");
    assert(verificarInvariantesTreap(raiz) && "Invariantes devem permanecer validas apos remocoes.");
    std::cout << "  -> Chaves " << chaveRemover1 << " e " << chaveRemover2 << " removidas com sucesso [OK]" << std::endl;
    std::cout << "  -> Invariantes pos-remocao: VALIDAS [OK]" << std::endl;

    std::cout << "[MEMORIA] Desalocando arvore Treap..." << std::endl;
    liberarTreap(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Teste da Treap finalizado com exito!" << std::endl;
    return 0;
}
