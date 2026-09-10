#include "patricia.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

void testarPatricia() {
    std::cout << "[TESTE] Iniciando testes da Árvore Patricia..." << std::endl;

    Patricia pat;
    assert(pat.vazia());
    assert(pat.tamanho() == 0);

    // 1. Inserção básica com aresta compactada
    pat.inserir("sol");
    pat.inserir("som");
    assert(pat.tamanho() == 2);
    assert(pat.buscar("sol"));
    assert(pat.buscar("som"));
    assert(!pat.buscar("so"));

    // 2. Inserção que divide aresta (cisão)
    pat.inserir("sal"); // Prefixo comum 's' -> bifurca em 'al' e 'o' -> 'l'/'m'
    assert(pat.tamanho() == 3);
    assert(pat.buscar("sal"));
    assert(pat.buscar("sol"));
    assert(pat.buscar("som"));

    // 3. Extensão de chave
    pat.inserir("soma");
    assert(pat.tamanho() == 4);
    assert(pat.buscar("soma"));

    // 4. Prefixo (comecaCom)
    assert(pat.comecaCom("so"));
    assert(pat.comecaCom("sa"));
    assert(pat.comecaCom("s"));
    assert(!pat.comecaCom("su"));

    // 5. Autocompletar
    auto res = pat.autocompletar("so");
    std::sort(res.begin(), res.end());
    assert(res.size() == 3);
    assert(res[0] == "sol");
    assert(res[1] == "som");
    assert(res[2] == "soma");

    // 6. Remoção e Fusão de Arestas
    bool remSal = pat.remover("sal");
    assert(remSal);
    assert(!pat.buscar("sal"));
    assert(pat.buscar("sol"));
    assert(pat.buscar("som"));
    assert(pat.buscar("soma"));
    assert(pat.tamanho() == 3);

    // Remoção de folha
    assert(pat.remover("soma"));
    assert(!pat.buscar("soma"));
    assert(pat.buscar("som"));
    assert(pat.tamanho() == 2);

    // Chave inexistente
    assert(!pat.remover("inexistente"));

    std::cout << "[TESTE] Árvore Patricia: Todos os testes passaram com sucesso!" << std::endl;
}

int main() {
    testarPatricia();
    return 0;
}
