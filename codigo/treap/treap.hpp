#ifndef TREAP_HPP
#define TREAP_HPP

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <climits>

class Treap {
public:
    struct No {
        int chave;
        int prioridade;
        No* esq = nullptr;
        No* dir = nullptr;

        No(int k, int p) : chave(k), prioridade(p) {}
        ~No() = default;
    };

private:
    No* raiz = nullptr;
    size_t totalElementos = 0;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    No* rotDir(No* y) {
        No* x = y->esq;
        y->esq = x->dir;
        x->dir = y;
        return x;
    }

    No* rotEsq(No* x) {
        No* y = x->dir;
        x->dir = y->esq;
        y->esq = x;
        return y;
    }

    No* inserirRecursivo(No* no, int chave, int prioridade, bool& inserido) {
        if (!no) {
            inserido = true;
            totalElementos++;
            return new No(chave, prioridade);
        }

        if (chave < no->chave) {
            no->esq = inserirRecursivo(no->esq, chave, prioridade, inserido);
            if (no->esq->prioridade < no->prioridade) {
                no = rotDir(no);
            }
        } else if (chave > no->chave) {
            no->dir = inserirRecursivo(no->dir, chave, prioridade, inserido);
            if (no->dir->prioridade < no->prioridade) {
                no = rotEsq(no);
            }
        } else {
            // Chave duplicada: nao insere
            inserido = false;
        }
        return no;
    }

    No* removerRecursivo(No* no, int chave, bool& removido) {
        if (!no) return nullptr;

        if (chave < no->chave) {
            no->esq = removerRecursivo(no->esq, chave, removido);
        } else if (chave > no->chave) {
            no->dir = removerRecursivo(no->dir, chave, removido);
        } else {
            removido = true;

            if (!no->esq && !no->dir) {
                delete no;
                return nullptr;
            } else if (!no->esq) {
                No* temp = no->dir;
                no->dir = nullptr;
                delete no;
                return temp;
            } else if (!no->dir) {
                No* temp = no->esq;
                no->esq = nullptr;
                delete no;
                return temp;
            } else {
                // Ambos os filhos existem: rotaciona com o de menor prioridade (min-heap)
                if (no->esq->prioridade < no->dir->prioridade) {
                    no = rotDir(no);
                    no->dir = removerRecursivo(no->dir, chave, removido);
                } else {
                    no = rotEsq(no);
                    no->esq = removerRecursivo(no->esq, chave, removido);
                }
            }
        }
        return no;
    }

    bool verificarInvariantesRecursivo(No* no, int minChave, int maxChave, int paiPrioridade) const {
        if (!no) return true;

        // Invariante de BST
        if (no->chave <= minChave || no->chave >= maxChave) return false;

        // Invariante de Min-Heap
        if (no->prioridade < paiPrioridade) return false;

        return verificarInvariantesRecursivo(no->esq, minChave, no->chave, no->prioridade) &&
               verificarInvariantesRecursivo(no->dir, no->chave, maxChave, no->prioridade);
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

    size_t calcularAlturaIterativo(No* r) const {
        if (!r) return 0;
        std::vector<No*> nivelAtual = {r};
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

    void emOrdem(No* no, std::vector<int>& res) const {
        if (!no) return;
        emOrdem(no->esq, res);
        res.push_back(no->chave);
        emOrdem(no->dir, res);
    }

public:
    Treap(unsigned int semente = 42)
        : rng(semente), dist(1, 1000000000) {}

    ~Treap() {
        limpar();
    }

    Treap(const Treap&) = delete;
    Treap& operator=(const Treap&) = delete;

    Treap(Treap&& other) noexcept
        : raiz(other.raiz), totalElementos(other.totalElementos), rng(other.rng), dist(other.dist) {
        other.raiz = nullptr;
        other.totalElementos = 0;
    }

    Treap& operator=(Treap&& other) noexcept {
        if (this != &other) {
            limpar();
            raiz = other.raiz;
            totalElementos = other.totalElementos;
            rng = other.rng;
            dist = other.dist;
            other.raiz = nullptr;
            other.totalElementos = 0;
        }
        return *this;
    }

    bool inserir(int chave, int prioridade = -1) {
        if (prioridade == -1) {
            prioridade = dist(rng);
        }
        bool inserido = false;
        raiz = inserirRecursivo(raiz, chave, prioridade, inserido);
        return inserido;
    }

    bool buscar(int chave) const {
        No* atual = raiz;
        while (atual) {
            if (chave < atual->chave) atual = atual->esq;
            else if (chave > atual->chave) atual = atual->dir;
            else return true;
        }
        return false;
    }

    bool remover(int chave) {
        bool removido = false;
        raiz = removerRecursivo(raiz, chave, removido);
        if (removido) totalElementos--;
        return removido;
    }

    // Divisao (Split): Divide a Treap em t1 (<= chave) e t2 (> chave)
    static void split(No* t, int chave, No*& t1, No*& t2) {
        if (!t) {
            t1 = t2 = nullptr;
        } else if (t->chave <= chave) {
            split(t->dir, chave, t->dir, t2);
            t1 = t;
        } else {
            split(t->esq, chave, t1, t->esq);
            t2 = t;
        }
    }

    // Fusao (Merge): Une t1 e t2 assumindo que todas as chaves de t1 < t2
    static void merge(No*& t, No* t1, No* t2) {
        if (!t1 || !t2) {
            t = t1 ? t1 : t2;
        } else if (t1->prioridade < t2->prioridade) {
            merge(t1->dir, t1->dir, t2);
            t = t1;
        } else {
            merge(t2->esq, t1, t2->esq);
            t = t2;
        }
    }

    bool verificarInvariantes() const {
        return verificarInvariantesRecursivo(raiz, INT_MIN, INT_MAX, INT_MIN);
    }

    size_t tamanho() const { return totalElementos; }
    bool vazia() const { return totalElementos == 0; }
    size_t altura() const { return calcularAlturaIterativo(raiz); }

    std::vector<int> listarEmOrdem() const {
        std::vector<int> res;
        emOrdem(raiz, res);
        return res;
    }

    No* getRaiz() const { return raiz; }
};

#endif // TREAP_HPP
