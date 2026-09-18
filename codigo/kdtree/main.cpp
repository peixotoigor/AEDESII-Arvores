#include "kdtree.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore KD-Tree (k=2)                    " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string caminho = "codigo/benchmark/dados/cenario4_pontos_kdtree.txt";
    if (argc > 1) {
        caminho = argv[1];
    } else {
        std::ifstream teste1(caminho);
        if (!teste1.is_open()) {
            caminho = "../benchmark/dados/cenario4_pontos_kdtree.txt";
            std::ifstream teste2(caminho);
            if (!teste2.is_open()) {
                caminho = "dados_teste_kdtree.txt";
            }
        }
    }

    std::cout << "[INFO] Lendo dados fisicos de: " << caminho << std::endl;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "[ERRO CRITICO] Nao foi possivel abrir o arquivo: " << caminho << std::endl;
        return 1;
    }

    std::string linha, secaoAtual = "";
    NoKD* raiz = nullptr;
    std::vector<PontoKD> pontosBalanceados;
    int qtdInseridos = 0;

    while (std::getline(arq, linha)) {
        while (!linha.empty() && (linha.back() == '\r' || linha.back() == ' ' || linha.back() == '\t')) {
            linha.pop_back();
        }
        if (linha.empty() || linha[0] == '#') continue;

        if (linha[0] == '[' && linha.back() == ']') {
            secaoAtual = linha;
            continue;
        }

        size_t posHash = linha.find('#');
        if (posHash != std::string::npos) {
            linha = linha.substr(0, posHash);
        }

        std::istringstream iss(linha);

        if (secaoAtual == "[PONTOS_INICIAIS_2D]" || secaoAtual == "[PONTO_CONFINADO]") {
            double x, y;
            if (iss >> x >> y) {
                raiz = inserirKD(raiz, {x, y});
                qtdInseridos++;
            }
        } else if (secaoAtual == "[BUSCAR_EXATO]") {
            double x, y;
            if (iss >> x >> y) {
                bool achou = buscarExatoKD(raiz, {x, y});
                std::cout << "  [BUSCA EXATA] Ponto (" << x << ", " << y << ") -> " << (achou ? "Encontrado [OK]" : "Nao encontrado [OK]") << std::endl;
                if (x == 0.0 && y == 0.0) {
                    assert(!achou && "Ponto (0,0) nao deveria existir.");
                } else {
                    assert(achou && "Ponto inserido deveria ser encontrado.");
                }
            }
        } else if (secaoAtual == "[RANGE_QUERY]") {
            double xmin, ymin, xmax, ymax;
            if (iss >> xmin >> ymin >> xmax >> ymax) {
                std::vector<PontoKD> regiao;
                buscaIntervaloKD(raiz, {xmin, ymin}, {xmax, ymax}, 0, regiao);
                std::cout << "  [RANGE QUERY] Janela [" << xmin << ", " << xmax << "] x [" 
                          << ymin << ", " << ymax << "] -> " << regiao.size() << " pontos dentro da regiao." << std::endl;
                for (const auto& p : regiao) {
                    std::cout << "    -> Ponto no retangulo: (" << p[0] << ", " << p[1] << ")" << std::endl;
                }
                assert(regiao.size() == 3 && "Range query retangular deveria conter exatamente 3 pontos.");
            }
        } else if (secaoAtual == "[1NN_CONSULTA]") {
            double qx, qy;
            if (iss >> qx >> qy) {
                size_t visitados = 0;
                PontoKD consulta = {qx, qy};
                PontoKD maisProximo = vizinhoMaisProximoKD(raiz, consulta, &visitados);
                std::cout << "  [1-NN] Consulta Q=(" << qx << ", " << qy << ") -> Vizinho encontrado: (" 
                          << maisProximo[0] << ", " << maisProximo[1] << ") com " << visitados << " nos visitados." << std::endl;
                assert(maisProximo[0] == 7.4 && maisProximo[1] == 4.5 && "Vizinho mais proximo esperado e C(7.4, 4.5).");
            }
        } else if (secaoAtual == "[PONTOS_CONSTRUCAO_BALANCEADA]") {
            double x, y;
            if (iss >> x >> y) {
                pontosBalanceados.push_back({x, y});
            }
        }
    }
    arq.close();

    std::cout << "[DADOS] Total de pontos dinamicos inseridos: " << qtdInseridos << std::endl;
    assert(contarNosKD(raiz) == (size_t)qtdInseridos);

    std::cout << "[ESTATICA] Testando construcao estatica balanceada via mediana..." << std::endl;
    assert(pontosBalanceados.size() == 6);
    NoKD* kdBal = construirBalanceadaKD(pontosBalanceados, 0, pontosBalanceados.size(), 0);
    assert(contarNosKD(kdBal) == 6);
    int altBal = alturaKD(kdBal);
    std::cout << "  -> Total de pontos: " << pontosBalanceados.size() << " | Altura: " << altBal << " (<= 4) [OK]" << std::endl;
    assert(altBal <= 4);

    std::cout << "[MEMORIA] Desalocando arvores KD-Tree..." << std::endl;
    liberarKD(raiz);
    raiz = nullptr;
    liberarKD(kdBal);
    kdBal = nullptr;

    std::cout << "[SUCESSO] Teste da KD-Tree finalizado com exito!" << std::endl;
    return 0;
}
