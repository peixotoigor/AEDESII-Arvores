#ifndef BST_HPP
#define BST_HPP

#include <iostream>
#include <algorithm>
#include <vector>

template <typename Key = int>
class BST {
public:
    struct No {
        Key chave;
        No* esq = nullptr;
        No* dir = nullptr;

        No(const Key& k) : chave(k) {}

        ~No() = default;
    };

private:
    No* raiz = nullptr;
    size_t totalElementos = 0;

    void limpar() {
        std::vector<No*> pilha;
        if (raiz) pilha.push_back(raiz);
        while (!pilha.empty()) {
            No* curr = pilha.back();
            pilha.pop_back();
            if (curr->esq) pilha.push_back(curr->esq);
            if (curr->dir) pilha.push_back(curr->dir);
            delete curr;
        }
        raiz = nullptr;
        totalElementos = 0;
    }

public:
    BST() = default;

    ~BST() {
        limpar();
    }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    void inserir(const Key& chave) {
        if (!raiz) {
            raiz = new No(chave);
            totalElementos++;
            return;
        }
        No* atual = raiz;
        while (true) {
            if (chave < atual->chave) {
                if (!atual->esq) {
                    atual->esq = new No(chave);
                    totalElementos++;
                    return;
                }
                atual = atual->esq;
            } else if (chave > atual->chave) {
                if (!atual->dir) {
                    atual->dir = new No(chave);
                    totalElementos++;
                    return;
                }
                atual = atual->dir;
            } else {
                return; // Duplicata
            }
        }
    }

    bool buscar(const Key& chave) const {
        No* atual = raiz;
        while (atual) {
            if (chave < atual->chave) atual = atual->esq;
            else if (chave > atual->chave) atual = atual->dir;
            else return true;
        }
        return false;
    }

    size_t tamanho() const { return totalElementos; }
    bool vazia() const { return totalElementos == 0; }

    size_t altura() const {
        if (!raiz) return 0;
        std::vector<No*> nivelAtual = {raiz};
        size_t alt = 0;
        while (!nivelAtual.empty()) {
            alt++;
            std::vector<No*> proximoNivel;
            for (No* n : nivelAtual) {
                if (n->esq) proximoNivel.push_back(n->esq);
                if (n->dir) proximoNivel.push_back(n->dir);
            }
            nivelAtual = std::move(proximoNivel);
        }
        return alt;
    }

    No* getRaiz() const { return raiz; }
};

#endif // BST_HPP
