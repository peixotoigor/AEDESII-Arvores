#include "kdtree.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testarKDTree() {
    std::cout << "[TESTE] Iniciando testes da Árvore KD-Tree..." << std::endl;

    KDTree kd(2);
    assert(kd.vazia());
    assert(kd.tamanho() == 0);

    // 1. Inserção incremental dos pontos da Figura 5 do relatório
    // A(5, 5.7), B(2.3, 3.0), C(7.4, 4.5), D(3.6, 1.4)
    kd.inserir({5.0, 5.7});
    kd.inserir({2.3, 3.0});
    kd.inserir({7.4, 4.5});
    kd.inserir({3.6, 1.4});
    assert(kd.tamanho() == 4);

    // Inserção de E(7.5, 2.0)
    kd.inserir({7.5, 2.0});
    assert(kd.tamanho() == 5);

    // 2. Busca exata
    assert(kd.buscarExato({5.0, 5.7}));
    assert(kd.buscarExato({7.5, 2.0}));
    assert(!kd.buscarExato({0.0, 0.0}));

    // 3. Busca por intervalo retangular
    // Intervalo x in [2.0, 6.0], y in [1.0, 6.0] -> Deve conter B, D, A
    auto pontosRegiao = kd.buscaIntervalo({2.0, 1.0}, {6.0, 6.0});
    assert(pontosRegiao.size() == 3);

    // 4. Busca de vizinho mais próximo (k-NN)
    size_t visitados = 0;
    auto maisProximo = kd.vizinhoMaisProximo({7.4, 4.4}, &visitados);
    // Deve ser o ponto C (7.4, 4.5)
    assert(maisProximo[0] == 7.4 && maisProximo[1] == 4.5);
    std::cout << "  Vizinho mais próximo de (7.4, 4.4): (" << maisProximo[0] << ", " << maisProximo[1]
              << "), nós visitados: " << visitados << " de " << kd.tamanho() << std::endl;

    // 5. Construção balanceada estática
    std::vector<KDTree::Ponto> pontos = {
        {2.0, 3.0}, {5.0, 4.0}, {9.0, 6.0}, {4.0, 7.0}, {8.0, 1.0}, {7.0, 2.0}
    };
    KDTree kdBal(2);
    kdBal.construirBalanceada(pontos);
    assert(kdBal.tamanho() == 6);
    assert(kdBal.altura() <= 4);

    std::cout << "[TESTE] Árvore KD-Tree: Todos os testes passaram com sucesso!" << std::endl;
}

int main() {
    testarKDTree();
    return 0;
}
