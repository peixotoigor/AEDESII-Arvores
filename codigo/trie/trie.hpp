#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

/*
  A struct NoTrie cria um novo tipo de dado para representar um nó na árvore Trie de prefixos.
  A árvore Trie não armazena a palavra inteira em cada nó: o caminho da raiz até um nó
  soletra os caracteres da palavra.
*/
struct NoTrie {
    bool terminal = false; // Indica se este nó marca o final de uma palavra válida
    std::unordered_map<char, NoTrie*> filhos; // Mapeia cada caractere para o ponteiro do nó filho

    /*
      filhos é uma tabela hash que associa um caractere a um ponteiro para outro nó (NoTrie).
      Se a palavra continuar com determinado caractere, acessamos o endereço do nó filho correspondente.
    */
};

/*
  Cria e aloca dinamicamente um novo nó da Trie na memória.
  Inicia o booleano terminal como falso.
*/
NoTrie* criarNoTrie() {
    NoTrie* novo = new NoTrie();
    novo->terminal = false;
    return novo;
}

/*
  Insere uma palavra na Trie caractere por caractere.
  Para cada letra da palavra, desce na árvore criando nós que ainda não existam.
*/
void inserirTrie(NoTrie* raiz, const std::string& chave) {
    if (raiz == nullptr) return;

    NoTrie* atual = raiz;
    for (char c : chave) {
        // Se ainda não existe transição para o caractere c, cria o novo nó filho
        if (atual->filhos.find(c) == atual->filhos.end()) {
            atual->filhos[c] = criarNoTrie();
        }
        // Avança o ponteiro para o nó correspondente ao caractere
        atual = atual->filhos[c];
    }
    // Marca o último nó visitado como final de uma palavra válida
    atual->terminal = true;
}

/*
  Busca uma palavra completa na Trie.
  Retorna true se todos os caracteres existirem no caminho e o último nó for terminal.
*/
bool buscarTrie(NoTrie* raiz, const std::string& chave) {
    if (raiz == nullptr) return false;

    NoTrie* atual = raiz;
    for (char c : chave) {
        auto it = atual->filhos.find(c);
        // Se o caractere não existir no caminho, a palavra não está na árvore
        if (it == atual->filhos.end()) {
            return false;
        }
        atual = it->second;
    }
    // Retorna true apenas se a palavra foi marcada como válida
    return atual->terminal;
}

/*
  Verifica se existe ao menos uma palavra na árvore que comece com o prefixo informado.
*/
bool comecaComTrie(NoTrie* raiz, const std::string& prefixo) {
    if (raiz == nullptr) return false;

    NoTrie* atual = raiz;
    for (char c : prefixo) {
        auto it = atual->filhos.find(c);
        // Se algum caractere do prefixo faltar, não existe correspondência
        if (it == atual->filhos.end()) {
            return false;
        }
        atual = it->second;
    }
    // Todos os caracteres do prefixo foram encontrados na árvore
    return true;
}

/*
  Função auxiliar recursiva para percorrer os ramos e listar palavras para autocompletar.
*/
void coletarPalavrasTrie(NoTrie* atual, std::string prefixo, std::vector<std::string>& resultado) {
    if (atual == nullptr) return;

    // Se este ponto do caminho é terminal, adiciona a palavra formada
    if (atual->terminal) {
        resultado.push_back(prefixo);
    }
    // Percorre recursivamente todos os nós filhos
    for (const auto& par : atual->filhos) {
        char c = par.first;
        NoTrie* filho = par.second;
        coletarPalavrasTrie(filho, prefixo + c, resultado);
    }
}

/*
  Retorna todas as palavras armazenadas na Trie que começam com o prefixo dado.
*/
std::vector<std::string> autocompletarTrie(NoTrie* raiz, const std::string& prefixo) {
    std::vector<std::string> resultado;
    if (raiz == nullptr) return resultado;

    NoTrie* atual = raiz;
    for (char c : prefixo) {
        auto it = atual->filhos.find(c);
        if (it == atual->filhos.end()) {
            return resultado; // Prefixo não existe na árvore
        }
        atual = it->second;
    }

    // A partir do nó onde o prefixo termina, coleta todas as palavras nas subárvores
    coletarPalavrasTrie(atual, prefixo, resultado);
    return resultado;
}

/*
  Função auxiliar recursiva para remoção de palavra da Trie.
*/
bool removerRecursivoTrie(NoTrie* atual, const std::string& chave, size_t indice, bool& sucesso) {
    if (atual == nullptr) return false;

    // Chegou ao fim da palavra
    if (indice == chave.size()) {
        if (!atual->terminal) return false; // Palavra não existia como terminal
        atual->terminal = false;
        sucesso = true;
        // Se o nó não possui filhos, pode ser desalocado da memória
        return atual->filhos.empty();
    }

    char c = chave[indice];
    auto it = atual->filhos.find(c);
    if (it == atual->filhos.end()) return false; // Caractere não existe

    bool deveDeletarFilho = removerRecursivoTrie(it->second, chave, indice + 1, sucesso);

    // Se o filho ficou vazio após a remoção, desaloca da memória
    if (deveDeletarFilho) {
        delete it->second;
        atual->filhos.erase(it);
        // O nó atual só pode ser apagado se não for terminal e não tiver outros filhos
        return !atual->terminal && atual->filhos.empty();
    }

    return false;
}

/*
  Remove uma palavra da Trie sem prejudicar prefixos e palavras irmãs.
*/
bool removerTrie(NoTrie* raiz, const std::string& chave) {
    if (raiz == nullptr) return false;
    if (chave.empty()) {
        if (raiz->terminal) {
            raiz->terminal = false;
            return true;
        }
        return false;
    }
    bool sucesso = false;
    removerRecursivoTrie(raiz, chave, 0, sucesso);
    return sucesso;
}

/*
  Conta recursivamente a quantidade total de nós alocados na memória pela Trie.
*/
size_t contarNosTrie(NoTrie* raiz) {
    if (raiz == nullptr) return 0;
    size_t cont = 1;
    for (const auto& par : raiz->filhos) {
        cont += contarNosTrie(par.second);
    }
    return cont;
}

/*
  Conta o total de palavras válidas armazenadas na Trie (nós terminais).
*/
size_t contarPalavrasTrie(NoTrie* raiz) {
    if (raiz == nullptr) return 0;
    size_t cont = raiz->terminal ? 1 : 0;
    for (const auto& par : raiz->filhos) {
        cont += contarPalavrasTrie(par.second);
    }
    return cont;
}

/*
  Libera a memória de todos os nós alocados recursivamente.
*/
void liberarTrie(NoTrie* raiz) {
    if (raiz != nullptr) {
        for (auto& par : raiz->filhos) {
            liberarTrie(par.second);
        }
        delete raiz;
    }
}

#endif // TRIE_HPP
