#include "splay.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testarSplay() {
    std::cout << "[TESTE] Iniciando testes da Árvore Splay..." << std::endl;

    SplayTree splay;
    assert(splay.vazia());
    assert(splay.tamanho() == 0);

    // 1. Inserção com promoção à raiz
    splay.inserir(50);
    assert(splay.getRaizChave() == 50);

    splay.inserir(30);
    assert(splay.getRaizChave() == 30);

    splay.inserir(70);
    assert(splay.getRaizChave() == 70);

    splay.inserir(20);
    assert(splay.getRaizChave() == 20);

    // 2. Busca com autoajuste
    assert(splay.buscar(50));
    assert(splay.getRaizChave() == 50); // 50 deve virar a nova raiz

    assert(!splay.buscar(100)); // Busca de elemento inexistente
    // O ultimo elemento inspecionado deve subir
    assert(splay.getRaizChave() == 70);

    // 3. Ordem simétrica de BST preservada
    auto elementos = splay.listarEmOrdem();
    assert(elementos.size() == 4);
    assert(elementos[0] == 20 && elementos[1] == 30 && elementos[2] == 50 && elementos[3] == 70);

    // 4. Remoção
    bool rem = splay.remover(50);
    assert(rem);
    assert(!splay.buscar(50));
    assert(splay.tamanho() == 3);

    auto aposRemocao = splay.listarEmOrdem();
    assert(aposRemocao.size() == 3);
    assert(aposRemocao[0] == 20 && aposRemocao[1] == 30 && aposRemocao[2] == 70);

    // Remoção da raiz atual
    int raizAtual = splay.getRaizChave();
    assert(splay.remover(raizAtual));
    assert(splay.tamanho() == 2);

    std::cout << "[TESTE] Árvore Splay: Todos os testes passaram com sucesso!" << std::endl;
}

int main() {
    testarSplay();
    return 0;
}
