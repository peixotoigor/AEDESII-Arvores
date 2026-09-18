#ifndef PATRICIA_HPP
#define PATRICIA_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>

struct NoPatricia;

/*
  A struct ArestaPatricia representa a aresta compactada que liga dois nós na Árvore Patricia.
  Em vez de criar um nó para cada caractere, guarda uma sequência de caracteres (rótulo)
  e o ponteiro para o nó de destino.
*/
struct ArestaPatricia {
    std::string rotulo; // Trecho de caracteres associado a esta aresta
    NoPatricia* destino = nullptr; // Ponteiro para o nó de destino da aresta
};

/*
  A struct NoPatricia cria um novo tipo de dado para representar um nó na Árvore Patricia compactada.
  Guarda se uma palavra termina neste ponto e a lista de arestas de saída.
*/
struct NoPatricia {
    bool terminal = false; // Indica se uma palavra válida termina neste nó
    std::unordered_map<char, ArestaPatricia> filhos; // Mapeia o primeiro caractere da aresta para a aresta de destino
};

/*
  Cria e aloca dinamicamente um novo nó da Árvore Patricia.
*/
NoPatricia* criarNoPatricia(bool terminal = false) {
    NoPatricia* novo = new NoPatricia();
    novo->terminal = terminal;
    return novo;
}

/*
  Calcula o tamanho do maior prefixo comum (LCP) entre duas strings a e b.
  Compara caractere por caractere enquanto forem iguais.
*/
size_t calcularLCPPatricia(const std::string& a, const std::string& b) {
    size_t p = 0;
    while (p < a.size() && p < b.size() && a[p] == b[p]) {
        p++;
    }
    return p;
}

/*
  Função auxiliar recursiva para inserir uma palavra com cisão (split) de arestas.
*/
void inserirRecursivoPatricia(NoPatricia* no, const std::string& resto) {
    // Se a palavra chegou ao fim, marca o nó atual como terminal
    if (resto.empty()) {
        no->terminal = true;
        return;
    }

    char c = resto[0];
    auto it = no->filhos.find(c);

    // Caso 1: Nenhuma aresta começa com o caractere c -> cria uma aresta direta
    if (it == no->filhos.end()) {
        no->filhos[c] = {resto, criarNoPatricia(true)};
        return;
    }

    ArestaPatricia& e = it->second;
    size_t p = calcularLCPPatricia(resto, e.rotulo);

    if (p == e.rotulo.size()) {
        // A chave cobre completamente o rótulo da aresta: continua no nó destino
        inserirRecursivoPatricia(e.destino, resto.substr(p));
    } else {
        // Caso 2: Cisão de aresta (o prefixo diverge no meio do rótulo existente)
        NoPatricia* meio = criarNoPatricia(false);
        std::string rotuloComum = e.rotulo.substr(0, p);
        std::string sufixoAntigo = e.rotulo.substr(p);

        // O nó do meio passa a apontar para o antigo destino com o sufixo restante
        meio->filhos[sufixoAntigo[0]] = {sufixoAntigo, e.destino};
        e = {rotuloComum, meio};

        if (p == resto.size()) {
            // A nova chave terminou exatamente no ponto de corte
            meio->terminal = true;
        } else {
            // A nova chave continua para um novo ramo a partir do nó intermediário
            std::string sufixoNovo = resto.substr(p);
            meio->filhos[sufixoNovo[0]] = {sufixoNovo, criarNoPatricia(true)};
        }
    }
}

/*
  Insere uma palavra na Árvore Patricia a partir da raiz.
*/
void inserirPatricia(NoPatricia* raiz, const std::string& chave) {
    if (raiz == nullptr) return;
    inserirRecursivoPatricia(raiz, chave);
}

/*
  Busca uma palavra completa na Árvore Patricia.
*/
bool buscarPatricia(NoPatricia* raiz, const std::string& chave) {
    if (raiz == nullptr) return false;
    if (chave.empty()) return raiz->terminal;

    NoPatricia* atual = raiz;
    std::string resto = chave;

    while (!resto.empty()) {
        char c = resto[0];
        auto it = atual->filhos.find(c);
        if (it == atual->filhos.end()) return false;

        const ArestaPatricia& e = it->second;
        size_t p = calcularLCPPatricia(resto, e.rotulo);

        // Se a chave não cobrir todo o rótulo da aresta, a palavra não existe
        if (p < e.rotulo.size()) {
            return false;
        }
        resto = resto.substr(p);
        atual = e.destino;
    }
    return atual != nullptr && atual->terminal;
}

/*
  Verifica se existe alguma palavra na árvore que comece com o prefixo dado.
*/
bool comecaComPatricia(NoPatricia* raiz, const std::string& prefixo) {
    if (raiz == nullptr) return false;
    if (prefixo.empty()) return true;

    NoPatricia* atual = raiz;
    std::string resto = prefixo;

    while (!resto.empty()) {
        char c = resto[0];
        auto it = atual->filhos.find(c);
        if (it == atual->filhos.end()) return false;

        const ArestaPatricia& e = it->second;
        size_t p = calcularLCPPatricia(resto, e.rotulo);

        if (p == resto.size()) {
            return true; // Prefixo foi completamente percorrido
        }
        if (p < e.rotulo.size()) {
            return false; // Divergiu antes de percorrer a aresta toda
        }
        resto = resto.substr(p);
        atual = e.destino;
    }
    return true;
}

/*
  Coleta recursivamente todas as palavras a partir de um nó para autocompletar.
*/
void coletarPalavrasPatricia(NoPatricia* atual, std::string prefixo, std::vector<std::string>& resultado) {
    if (atual == nullptr) return;
    if (atual->terminal) {
        resultado.push_back(prefixo);
    }
    for (const auto& par : atual->filhos) {
        const ArestaPatricia& aresta = par.second;
        coletarPalavrasPatricia(aresta.destino, prefixo + aresta.rotulo, resultado);
    }
}

/*
  Retorna todas as palavras armazenadas que compartilham o prefixo informado.
*/
std::vector<std::string> autocompletarPatricia(NoPatricia* raiz, const std::string& prefixo) {
    std::vector<std::string> resultado;
    if (raiz == nullptr) return resultado;

    if (prefixo.empty()) {
        coletarPalavrasPatricia(raiz, "", resultado);
        return resultado;
    }

    NoPatricia* atual = raiz;
    std::string resto = prefixo;
    std::string caminhoAcumulado = "";

    while (!resto.empty()) {
        char c = resto[0];
        auto it = atual->filhos.find(c);
        if (it == atual->filhos.end()) return resultado;

        const ArestaPatricia& e = it->second;
        size_t p = calcularLCPPatricia(resto, e.rotulo);

        if (p == resto.size()) {
            caminhoAcumulado += e.rotulo;
            coletarPalavrasPatricia(e.destino, caminhoAcumulado, resultado);
            return resultado;
        }
        if (p < e.rotulo.size()) {
            return resultado; // Divergiu
        }
        caminhoAcumulado += e.rotulo;
        resto = resto.substr(p);
        atual = e.destino;
    }

    coletarPalavrasPatricia(atual, caminhoAcumulado, resultado);
    return resultado;
}

/*
  Função auxiliar recursiva para remoção com fusão de arestas.
*/
bool removerRecursivoPatricia(NoPatricia* no, const std::string& resto, bool& sucesso) {
    if (no == nullptr) return false;

    if (resto.empty()) {
        if (!no->terminal) return false;
        no->terminal = false;
        sucesso = true;
        return no->filhos.empty();
    }

    char c = resto[0];
    auto it = no->filhos.find(c);
    if (it == no->filhos.end()) return false;

    ArestaPatricia& e = it->second;
    size_t p = calcularLCPPatricia(resto, e.rotulo);

    if (p != e.rotulo.size()) {
        return false;
    }

    bool deveDeletarDestino = removerRecursivoPatricia(e.destino, resto.substr(p), sucesso);

    if (deveDeletarDestino) {
        delete e.destino;
        no->filhos.erase(it);
    } else if (e.destino != nullptr && !e.destino->terminal && e.destino->filhos.size() == 1) {
        // Fusão de arestas: contrai o nó intermediário desnecessário
        auto filhoUnicoIt = e.destino->filhos.begin();
        ArestaPatricia subAresta = filhoUnicoIt->second;
        e.rotulo += subAresta.rotulo;
        NoPatricia* noDescendente = subAresta.destino;

        e.destino->filhos.clear();
        delete e.destino;
        e.destino = noDescendente;
    }

    return !no->terminal && no->filhos.empty();
}

/*
  Remove uma chave da Árvore Patricia.
*/
bool removerPatricia(NoPatricia* raiz, const std::string& chave) {
    if (raiz == nullptr) return false;
    if (chave.empty()) {
        if (raiz->terminal) {
            raiz->terminal = false;
            return true;
        }
        return false;
    }
    bool sucesso = false;
    removerRecursivoPatricia(raiz, chave, sucesso);
    return sucesso;
}

/*
  Conta recursivamente o total de nós alocados na Árvore Patricia.
*/
size_t contarNosPatricia(NoPatricia* raiz) {
    if (raiz == nullptr) return 0;
    size_t cont = 1;
    for (const auto& par : raiz->filhos) {
        cont += contarNosPatricia(par.second.destino);
    }
    return cont;
}

/*
  Conta o total de palavras válidas armazenadas na árvore.
*/
size_t contarPalavrasPatricia(NoPatricia* raiz) {
    if (raiz == nullptr) return 0;
    size_t cont = raiz->terminal ? 1 : 0;
    for (const auto& par : raiz->filhos) {
        cont += contarPalavrasPatricia(par.second.destino);
    }
    return cont;
}

/*
  Libera a memória de todos os nós alocados recursivamente.
*/
void liberarPatricia(NoPatricia* raiz) {
    if (raiz != nullptr) {
        for (auto& par : raiz->filhos) {
            liberarPatricia(par.second.destino);
        }
        delete raiz;
    }
}

#endif // PATRICIA_HPP
