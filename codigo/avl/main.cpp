#include "avl.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore AVL                              " << std::endl;
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

    NoAVL* raiz = nullptr;
    int valor;
    int qtdLidos = 0;
    const int LIMITE_TESTE = 2000; // Valida os primeiros 2000 inteiros ordenados

    while (arq >> valor && qtdLidos < LIMITE_TESTE) {
        raiz = inserirAVL(raiz, valor);
        qtdLidos++;
    }
    arq.close();

    std::cout << "[DADOS] Total de chaves inseridas: " << qtdLidos << std::endl;
    int h = obterAlturaAVL(raiz);
    int fb = fatorBalancoAVL(raiz);
    std::cout << "[ESTRUTURA] Altura da AVL equilibrada: " << h << " (cota teorica log2(2000) ~ 11 a 15)" << std::endl;
    std::cout << "[ESTRUTURA] Fator de balanco da raiz: " << fb << std::endl;

    assert(h <= 16 && "Sob insercao ordenada, a AVL deve preservar altura estritamente logaritmica.");
    assert(fb >= -1 && fb <= 1 && "Fator de balanco da AVL deve pertencer a {-1, 0, +1}.");

    std::cout << "[BUSCA] Testando buscas pontuais:" << std::endl;
    assert(buscarAVL(raiz, 1) && "Chave 1 deveria existir.");
    assert(buscarAVL(raiz, qtdLidos / 2) && "Chave intermediaria deveria existir.");
    assert(buscarAVL(raiz, qtdLidos) && "Ultima chave deveria existir.");
    assert(!buscarAVL(raiz, 999999) && "Chave 999999 nao deveria existir.");
    std::cout << "  -> Chave 1: Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << (qtdLidos / 2) << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave " << qtdLidos << ": Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave 999999: Nao encontrada [OK]" << std::endl;

    std::cout << "[REMOCAO] Testando remocao de chave:" << std::endl;
    raiz = removerAVL(raiz, 1);
    assert(!buscarAVL(raiz, 1) && "Chave 1 foi removida.");
    std::cout << "  -> Remocao da chave 1 concluida com sucesso [OK]" << std::endl;

    std::cout << "[MEMORIA] Liberando arvore AVL..." << std::endl;
    liberarAVL(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Todos os testes da AVL foram aprovados com dados do benchmark!" << std::endl;
    return 0;
}
