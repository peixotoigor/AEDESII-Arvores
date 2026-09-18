#include "trie.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>

int main(int argc, char* argv[]) {
    std::cout << "=================================================" << std::endl;
    std::cout << "  Teste: Arvore Trie                             " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string caminho = "codigo/benchmark/dados/cenario1_vocabulario_real.txt";
    if (argc > 1) {
        caminho = argv[1];
    } else {
        std::ifstream teste(caminho);
        if (!teste.is_open()) {
            caminho = "../benchmark/dados/cenario1_vocabulario_real.txt";
        }
    }

    std::cout << "[INFO] Lendo dados fisicos de: " << caminho << std::endl;
    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cerr << "[ERRO] Nao foi possivel abrir o arquivo: " << caminho << std::endl;
        return 1;
    }

    NoTrie* raiz = criarNoTrie();
    std::string palavra;
    std::vector<std::string> amostra;
    const size_t LIMITE_TESTE = 2000;

    while (std::getline(arq, palavra) && amostra.size() < LIMITE_TESTE) {
        while (!palavra.empty() && (palavra.back() == '\r' || palavra.back() == ' ' || palavra.back() == '\t')) {
            palavra.pop_back();
        }
        if (palavra.empty() || palavra[0] == '#') continue;
        inserirTrie(raiz, palavra);
        amostra.push_back(palavra);
    }
    arq.close();

    std::cout << "[DADOS] Total de palavras reais inseridas na Trie: " << amostra.size() << std::endl;
    std::cout << "[ESTRUTURA] Total de nos alocados na Trie: " << contarNosTrie(raiz) << std::endl;

    std::cout << "[BUSCA] Validando presenca de palavras inseridas:" << std::endl;
    assert(!amostra.empty() && "Amostra nao pode ser vazia.");
    assert(buscarTrie(raiz, amostra[0]) && "Primeira palavra inserida deve existir.");
    assert(buscarTrie(raiz, amostra[amostra.size() / 2]) && "Palavra intermediaria deve existir.");
    assert(buscarTrie(raiz, amostra.back()) && "Ultima palavra deve existir.");
    assert(!buscarTrie(raiz, "xyz_palavra_inexistente_12345") && "Palavra inexistente nao deve existir.");
    std::cout << "  -> \"" << amostra[0] << "\": Encontrada [OK]" << std::endl;
    std::cout << "  -> \"" << amostra[amostra.size() / 2] << "\": Encontrada [OK]" << std::endl;
    std::cout << "  -> \"" << amostra.back() << "\": Encontrada [OK]" << std::endl;
    std::cout << "  -> Inexistente: Nao encontrada [OK]" << std::endl;

    std::cout << "[PREFIXO] Validando teste de prefixos e autocompletar:" << std::endl;
    std::string prefixoTeste = amostra[0].substr(0, std::min<size_t>(3, amostra[0].size()));
    assert(comecaComTrie(raiz, prefixoTeste) && "Prefixo comum deve ser reconhecido.");
    auto sugestoes = autocompletarTrie(raiz, prefixoTeste);
    std::cout << "  -> Prefixo \"" << prefixoTeste << "\" possui " << sugestoes.size() << " sugestoes no autocompletar [OK]" << std::endl;

    std::cout << "[REMOCAO] Removendo palavra da amostra..." << std::endl;
    std::string palavraRemover = amostra[0];
    removerTrie(raiz, palavraRemover);
    assert(!buscarTrie(raiz, palavraRemover) && "Palavra removida nao deve ser encontrada.");
    std::cout << "  -> \"" << palavraRemover << "\" removida com sucesso [OK]" << std::endl;

    std::cout << "[MEMORIA] Liberando arvore Trie..." << std::endl;
    liberarTrie(raiz);
    raiz = nullptr;

    std::cout << "[SUCESSO] Todos os testes da Trie foram aprovados com dados do benchmark!" << std::endl;
    return 0;
}
