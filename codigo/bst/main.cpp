#include "bst.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore Binaria de Busca (BST)           " << std::endl;
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

    NoBST* raiz = nullptr;
    int valor;
    int qtdLidos = 0;
    const int LIMITE_TESTE = 2000; // Valida os primeiros 2000 inteiros ordenados

    while (arq >> valor && qtdLidos < LIMITE_TESTE) {
        raiz = inserirBST(raiz, valor);
        qtdLidos++;
    }
    arq.close();

    std::cout << "[DADOS] Total de chaves ordenadas inseridas: " << qtdLidos << std::endl;
    int nos = contarNosBST(raiz);
    int h = alturaBST(raiz);
    std::cout << "[ESTRUTURA] Total de nos contabilizados: " << nos << std::endl;
    std::cout << "[ESTRUTURA] Altura da arvore (degeneracao linear): " << h << std::endl;

    assert(nos == qtdLidos && "O total de nos deve ser igual a quantidade de elementos inseridos.");
    assert(h == qtdLidos && "Sob insercao ordenada, a BST deve degenerar para altura exatamente igual a n.");

    std::cout << "[BUSCA] Testando buscas pontuais:" << std::endl;
    assert(buscarBST(raiz, 1) && "Chave 1 deveria existir.");
    assert(buscarBST(raiz, qtdLidos / 2) && "Chave intermediaria deveria existir.");
    assert(buscarBST(raiz, qtdLidos) && "Ultima chave deveria existir.");
    assert(!buscarBST(raiz, 999999) && "Chave 999999 nao deveria existir.");
    std::cout << "  -> Chave 1: Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << (qtdLidos / 2) << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << qtdLidos << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave 999999: Nao encontrada [OK]" << std::endl;

    std::cout << "[MEMORIA] Liberando arvore recursivamente..." << std::endl;
    liberarBST(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Todos os testes da BST foram aprovados com dados do benchmark!" << std::endl;
    return 0;
}
