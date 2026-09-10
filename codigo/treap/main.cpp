#include "treap.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testarTreap() {
    std::cout << "[TESTE] Iniciando testes da Árvore Treap..." << std::endl;

    Treap treap(12345);
    assert(treap.vazia());
    assert(treap.tamanho() == 0);

    // 1. Inserção determinística controlada (exemplo do relatório: Figura 4)
    // 40/15, 20/30, 60/25
    treap.inserir(40, 15);
    treap.inserir(20, 30);
    treap.inserir(60, 25);
    assert(treap.tamanho() == 3);
    assert(treap.verificarInvariantes());

    // Inserção de 50 com prioridade alta (10) violando min-heap
    treap.inserir(50, 10);
    assert(treap.tamanho() == 4);
    assert(treap.verificarInvariantes());
    // 50/10 deve ter subido à raiz
    assert(treap.getRaiz()->chave == 50);
    assert(treap.getRaiz()->prioridade == 10);

    // 2. Busca
    assert(treap.buscar(40));
    assert(treap.buscar(20));
    assert(treap.buscar(60));
    assert(treap.buscar(50));
    assert(!treap.buscar(999));

    // 3. Remoção mantendo invariantes
    assert(treap.remover(50)); // remove a raiz
    assert(!treap.buscar(50));
    assert(treap.tamanho() == 3);
    assert(treap.verificarInvariantes());

    // 4. Inserção em massa sequencial (teste do balanceamento probabilístico)
    Treap treapGrande(42);
    for (int i = 1; i <= 1000; i++) {
        treapGrande.inserir(i);
    }
    assert(treapGrande.tamanho() == 1000);
    assert(treapGrande.verificarInvariantes());
    // A altura de uma BST desbalanceada seria 1000; na Treap deve ser ~ 2 a 3 * log2(1000) ~ 20-30
    size_t alt = treapGrande.altura();
    std::cout << "  Altura para N=1000 inseridos em ordem: " << alt << " (esperado << 1000)" << std::endl;
    assert(alt < 40);

    std::cout << "[TESTE] Árvore Treap: Todos os testes passaram com sucesso!" << std::endl;
}

int main() {
    testarTreap();
    return 0;
}
