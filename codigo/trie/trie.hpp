#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <stdexcept>

class Trie {
public:
    struct No {
        bool terminal = false;
        std::unordered_map<char, No*> filhos;

        ~No() {
            for (auto& [c, p] : filhos) {
                delete p;
            }
        }
    };

private:
    No* raiz = nullptr;
    size_t totalChaves = 0;

    bool removerRecursivo(No* atual, const std::string& chave, size_t indice, bool& sucesso) {
        if (!atual) return false;

        if (indice == chave.size()) {
            if (!atual->terminal) return false; // Chave inexistente como terminal
            atual->terminal = false;
            totalChaves--;
            sucesso = true;
            return atual->filhos.empty(); // Se nao tem filhos, pode ser desalocado
        }

        char c = chave[indice];
        auto it = atual->filhos.find(c);
        if (it == atual->filhos.end()) return false;

        bool deveDeletarFilho = removerRecursivo(it->second, chave, indice + 1, sucesso);

        if (deveDeletarFilho) {
            delete it->second;
            atual->filhos.erase(it);
            return !atual->terminal && atual->filhos.empty();
        }

        return false;
    }

    void coletarPalavras(No* atual, std::string prefixo, std::vector<std::string>& resultado) const {
        if (!atual) return;
        if (atual->terminal) {
            resultado.push_back(prefixo);
        }
        for (const auto& [c, filho] : atual->filhos) {
            coletarPalavras(filho, prefixo + c, resultado);
        }
    }

    size_t contarNosRecursivo(No* atual) const {
        if (!atual) return 0;
        size_t cont = 1;
        for (const auto& [c, filho] : atual->filhos) {
            cont += contarNosRecursivo(filho);
        }
        return cont;
    }

public:
    Trie() : raiz(new No()) {}

    ~Trie() {
        delete raiz;
    }

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    Trie(Trie&&) noexcept = default;
    Trie& operator=(Trie&&) noexcept = default;

    void inserir(const std::string& chave) {
        if (chave.empty()) {
            if (!raiz->terminal) {
                raiz->terminal = true;
                totalChaves++;
            }
            return;
        }
        No* atual = raiz;
        for (char c : chave) {
            if (!atual->filhos.count(c)) {
                atual->filhos[c] = new No();
            }
            atual = atual->filhos[c];
        }
        if (!atual->terminal) {
            atual->terminal = true;
            totalChaves++;
        }
    }

    bool buscar(const std::string& chave) const {
        if (chave.empty()) return raiz->terminal;
        No* atual = raiz;
        for (char c : chave) {
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return false;
            atual = it->second;
        }
        return atual->terminal;
    }

    bool comecaCom(const std::string& prefixo) const {
        No* atual = raiz;
        for (char c : prefixo) {
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return false;
            atual = it->second;
        }
        return true;
    }

    bool remover(const std::string& chave) {
        if (chave.empty()) {
            if (raiz->terminal) {
                raiz->terminal = false;
                totalChaves--;
                return true;
            }
            return false;
        }
        bool sucesso = false;
        removerRecursivo(raiz, chave, 0, sucesso);
        return sucesso;
    }

    std::vector<std::string> autocompletar(const std::string& prefixo) const {
        std::vector<std::string> resultado;
        No* atual = raiz;
        for (char c : prefixo) {
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return resultado;
            atual = it->second;
        }
        coletarPalavras(atual, prefixo, resultado);
        return resultado;
    }

    size_t tamanho() const { return totalChaves; }
    bool vazia() const { return totalChaves == 0; }
    size_t contarNos() const { return contarNosRecursivo(raiz); }

    No* getRaiz() const { return raiz; }
};

#endif // TRIE_HPP
