#ifndef AVL_HPP
#define AVL_HPP

#include <iostream>
#include <algorithm>
#include <vector>

template <typename Key = int>
class AVL {
public:
    struct No {
        Key chave;
        int altura = 1;
        No* esq = nullptr;
        No* dir = nullptr;

        No(const Key& k) : chave(k) {}
        ~No() = default;
    };

private:
    No* raiz = nullptr;
    size_t totalElementos = 0;

    int getAltura(No* no) const {
        return no ? no->altura : 0;
    }

    int fatorBalanco(No* no) const {
        return no ? getAltura(no->dir) - getAltura(no->esq) : 0;
    }

    void atualizarAltura(No* no) {
        if (no) {
            no->altura = 1 + std::max(getAltura(no->esq), getAltura(no->dir));
        }
    }

    No* rotDir(No* y) {
        No* x = y->esq;
        No* T2 = x->dir;

        x->dir = y;
        y->esq = T2;

        atualizarAltura(y);
        atualizarAltura(x);

        return x;
    }

    No* rotEsq(No* x) {
        No* y = x->dir;
        No* T2 = y->esq;

        y->esq = x;
        x->dir = T2;

        atualizarAltura(x);
        atualizarAltura(y);

        return y;
    }

    No* inserirRecursivo(No* no, const Key& chave) {
        if (!no) {
            totalElementos++;
            return new No(chave);
        }

        if (chave < no->chave) {
            no->esq = inserirRecursivo(no->esq, chave);
        } else if (chave > no->chave) {
            no->dir = inserirRecursivo(no->dir, chave);
        } else {
            return no; // Chaves duplicadas nao inseridas
        }

        atualizarAltura(no);
        int fb = fatorBalanco(no);

        // Caso Esquerda-Esquerda
        if (fb < -1 && chave < no->esq->chave) {
            return rotDir(no);
        }
        // Caso Direita-Direita
        if (fb > 1 && chave > no->dir->chave) {
            return rotEsq(no);
        }
        // Caso Esquerda-Direita
        if (fb < -1 && chave > no->esq->chave) {
            no->esq = rotEsq(no->esq);
            return rotDir(no);
        }
        // Caso Direita-Esquerda
        if (fb > 1 && chave < no->dir->chave) {
            no->dir = rotDir(no->dir);
            return rotEsq(no);
        }

        return no;
    }

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
    AVL() = default;

    ~AVL() {
        limpar();
    }

    AVL(const AVL&) = delete;
    AVL& operator=(const AVL&) = delete;

    void inserir(const Key& chave) {
        raiz = inserirRecursivo(raiz, chave);
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
    size_t altura() const { return getAltura(raiz); }

    No* getRaiz() const { return raiz; }
};

#endif // AVL_HPP
