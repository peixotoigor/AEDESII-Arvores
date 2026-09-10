#include "trie.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

void testarTrie() {
    std::cout << "[TESTE] Iniciando testes da Árvore Trie..." << std::endl;

    Trie trie;
    assert(trie.vazia());
    assert(trie.tamanho() == 0);
    assert(!trie.buscar("sol"));

    // 1. Inserção básica
    trie.inserir("sol");
    trie.inserir("som");
    trie.inserir("soma");
    assert(trie.tamanho() == 3);
    assert(trie.buscar("sol"));
    assert(trie.buscar("som"));
    assert(trie.buscar("soma"));
    assert(!trie.buscar("so")); // prefixo mas nao terminal

    // 2. Prefixo (comecaCom)
    assert(trie.comecaCom("so"));
    assert(trie.comecaCom("sol"));
    assert(trie.comecaCom("soma"));
    assert(!trie.comecaCom("sub"));

    // 3. Autocompletar
    auto sugestoes = trie.autocompletar("so");
    std::sort(sugestoes.begin(), sugestoes.end());
    assert(sugestoes.size() == 3);
    assert(sugestoes[0] == "sol");
    assert(sugestoes[1] == "som");
    assert(sugestoes[2] == "soma");

    // 4. Remoção parcial (remover "sol", "som" e "soma" devem permanecer)
    bool rem = trie.remover("sol");
    assert(rem);
    assert(!trie.buscar("sol"));
    assert(trie.buscar("som"));
    assert(trie.buscar("soma"));
    assert(trie.tamanho() == 2);

    // 5. Casos de borda
    trie.inserir("");
    assert(trie.buscar(""));
    assert(trie.remover(""));
    assert(!trie.buscar(""));

    // Remoção de chave inexistente
    assert(!trie.remover("inexistente"));

    std::cout << "[TESTE] Árvore Trie: Todos os testes passaram com sucesso!" << std::endl;
}

int main() {
    testarTrie();
    return 0;
}
