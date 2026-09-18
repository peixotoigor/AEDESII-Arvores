#include "splay.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore Splay                            " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string caminhoBase = "codigo/benchmark/dados/cenario2_sequencia_ordenada.txt";
    std::string caminhoZipf = "codigo/benchmark/dados/cenario3_consultas_zipf_80_20.txt";
    if (argc > 1) {
        caminhoBase = argv[1];
    } else {
        std::ifstream teste(caminhoBase);
        if (!teste.is_open()) {
            caminhoBase = "../benchmark/dados/cenario2_sequencia_ordenada.txt";
            caminhoZipf = "../benchmark/dados/cenario3_consultas_zipf_80_20.txt";
        }
    }

    std::cout << "[INFO] Lendo dados de insercao de: " << caminhoBase << std::endl;
    std::ifstream arqBase(caminhoBase);
    if (!arqBase.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir o arquivo: " << caminhoBase << std::endl;
        return 1;
    }

    NoSplay* raiz = nullptr;
    int val;
    int qtdInseridos = 0;
    const int LIMITE_INSERCAO = 1000;

    while (arqBase >> val && qtdInseridos < LIMITE_INSERCAO) {
        inserirSplay(raiz, val);
        qtdInseridos++;
    }
    arqBase.close();

    std::cout << "[DADOS] Total de chaves inseridas na Splay: " << qtdInseridos << std::endl;
    assert(raiz != nullptr && "Raiz nao pode ser nula apos insercoes.");
    std::cout << "[ESTRUTURA] Raiz atual apos insercoes (ultimo inserido): " << raiz->chave << std::endl;

    std::cout << "[INFO] Lendo consultas de Zipf (80/20) de: " << caminhoZipf << std::endl;
    std::ifstream arqZipf(caminhoZipf);
    int consultasValidadas = 0;
    if (arqZipf.is_open()) {
        int chaveConsulta;
        const int LIMITE_CONSULTAS = 2000;
        while (arqZipf >> chaveConsulta && consultasValidadas < LIMITE_CONSULTAS) {
            if (chaveConsulta >= 1 && chaveConsulta <= LIMITE_INSERCAO) {
                bool achou = buscarSplay(raiz, chaveConsulta);
                assert(achou && "Chave dentro do intervalo inserido deve ser encontrada.");
                assert(raiz->chave == chaveConsulta && "O no consultado DEVE ser promovido para a raiz (splay)!");
                consultasValidadas++;
            }
        }
        arqZipf.close();
        std::cout << "[AUTO-AJUSTE] Consultas Zipf processadas com sucesso com promocao a raiz: " << consultasValidadas << " [OK]" << std::endl;
    }

    std::cout << "[REMOCAO] Testando remocao de elemento na Splay:" << std::endl;
    removerSplay(raiz, 500);
    assert(!buscarSplay(raiz, 500) && "Chave removida nao deve ser encontrada.");
    std::cout << "  -> Chave 500 removida com sucesso [OK]" << std::endl;

    std::cout << "[MEMORIA] Liberando memoria da arvore Splay..." << std::endl;
    liberarSplay(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Todos os testes da Splay foram aprovados com dados do benchmark!" << std::endl;
    return 0;
}
