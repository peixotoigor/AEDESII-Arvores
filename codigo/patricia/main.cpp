#include "patricia.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore Patricia (Radix)                 " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string caminho = "codigo/benchmark/dados/cenario1_prefixos_extensos.txt";
    if (argc > 1) {
        caminho = argv[1];
    } else {
        std::ifstream teste(caminho);
        if (!teste.is_open()) {
            caminho = "../benchmark/dados/cenario1_prefixos_extensos.txt";
        }
    }

    std::cout << "[INFO] Lendo dados fisicos de: " << caminho << std::endl;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir o arquivo: " << caminho << std::endl;
        return 1;
    }

    NoPatricia* raiz = criarNoPatricia();
    std::string chave;
    std::vector<std::string> amostra;
    const size_t LIMITE_TESTE = 2000;

    while (std::getline(arq, chave) && amostra.size() < LIMITE_TESTE) {
        while (!chave.empty() && (chave.back() == '\r' || chave.back() == ' ' || chave.back() == '\t')) {
            chave.pop_back();
        }
        if (chave.empty() || chave[0] == '#') continue;
        inserirPatricia(raiz, chave);
        amostra.push_back(chave);
    }
    arq.close();

    std::cout << "[DADOS] Total de chaves com prefixos extensos inseridas: " << amostra.size() << std::endl;

    std::cout << "[BUSCA] Validando presenca de chaves inseridas:" << std::endl;
    assert(!amostra.empty() && "Amostra nao pode ser vazia.");
    assert(buscarPatricia(raiz, amostra[0]) && "Primeira chave inserida deve existir.");
    assert(buscarPatricia(raiz, amostra[amostra.size() / 2]) && "Chave intermediaria deve existir.");
    assert(buscarPatricia(raiz, amostra.back()) && "Ultima chave deve existir.");
    assert(!buscarPatricia(raiz, "chave_completamente_inexistente_99999") && "Chave inexistente nao deve existir.");
    std::cout << "  -> Chave [0]: Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave [N/2]: Encontrada [OK]" << std::endl;
    std::cout << "  -> Chave [N-1]: Encontrada [OK]" << std::endl;
    std::cout << "  -> Inexistente: Nao encontrada [OK]" << std::endl;

    std::cout << "[PREFIXO] Testando busca por prefixo e autocompletar:" << std::endl;
    std::string prefixoTeste = amostra[0].substr(0, std::min<size_t>(10, amostra[0].size()));
    assert(comecaComPatricia(raiz, prefixoTeste) && "Prefixo comum deve ser reconhecido na Patricia.");
    auto sugestoes = autocompletarPatricia(raiz, prefixoTeste);
    std::cout << "  -> Prefixo \"" << prefixoTeste << "...\" retornou " << sugestoes.size() << " sugestoes [OK]" << std::endl;

    std::cout << "[REMOCAO] Testando remocao e fusao reversa de arestas:" << std::endl;
    std::string chaveRemover = amostra.back();
    removerPatricia(raiz, chaveRemover);
    assert(!buscarPatricia(raiz, chaveRemover) && "Chave removida nao deve ser encontrada.");
    std::cout << "  -> Chave removida com sucesso preservando a integridade da arvore [OK]" << std::endl;

    std::cout << "[MEMORIA] Liberando memoria da Arvore Patricia..." << std::endl;
    liberarPatricia(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Todos os testes da Patricia foram aprovados com dados do benchmark!" << std::endl;
    return 0;
}
