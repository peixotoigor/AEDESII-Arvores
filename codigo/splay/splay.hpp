#ifndef SPLAY_HPP
#define SPLAY_HPP

#include <iostream>
#include <vector>
#include <algorithm>

class SplayTree {
public:
    struct No {
        int chave;
        No *esq = nullptr;
        No *dir = nullptr;
        No *pai = nullptr;

        No(int k) : chave(k) {}
        ~No() = default;
    };

private:
    No* raiz = nullptr;
    size_t totalElementos = 0;
    size_t contagemRotacoes = 0;

    void promover(No* x) {
        No* p = x->pai;
        if (!p) return;
        No* g = p->pai;

        contagemRotacoes++;

        if (x == p->esq) {
            p->esq = x->dir;
            if (x->dir) x->dir->pai = p;
            x->dir = p;
        } else {
            p->dir = x->esq;
            if (x->esq) x->esq->pai = p;
            x->esq = p;
        }

        p->pai = x;
        x->pai = g;

        if (g) {
            if (p == g->esq) g->esq = x;
            else g->dir = x;
        } else {
            raiz = x;
        }
    }

    void splay(No* x) {
        if (!x) return;
        while (x->pai) {
            No* p = x->pai;
            No* g = p->pai;
            if (!g) {
                promover(x); // zig
            } else if ((x == p->esq) == (p == g->esq)) {
                promover(p); // zig-zig
                promover(x);
            } else {
                promover(x); // zig-zag
                promover(x);
            }
        }
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
    SplayTree() = default;

    ~SplayTree() {
        limpar();
    }

    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;

    SplayTree(SplayTree&& other) noexcept
        : raiz(other.raiz), totalElementos(other.totalElementos), contagemRotacoes(other.contagemRotacoes) {
        other.raiz = nullptr;
        other.totalElementos = 0;
        other.contagemRotacoes = 0;
    }

    SplayTree& operator=(SplayTree&& other) noexcept {
        if (this != &other) {
            limpar();
            raiz = other.raiz;
            totalElementos = other.totalElementos;
            contagemRotacoes = other.contagemRotacoes;
            other.raiz = nullptr;
            other.totalElementos = 0;
            other.contagemRotacoes = 0;
        }
        return *this;
    }

    void inserir(int chave) {
        if (!raiz) {
            raiz = new No(chave);
            totalElementos++;
            return;
        }

        No* atual = raiz;
        No* paiAtual = nullptr;

        while (atual) {
            paiAtual = atual;
            if (chave < atual->chave) {
                atual = atual->esq;
            } else if (chave > atual->chave) {
                atual = atual->dir;
            } else {
                // Chave duplicada: apenas executa o splay
                splay(atual);
                return;
            }
        }

        No* novo = new No(chave);
        novo->pai = paiAtual;
        if (chave < paiAtual->chave) {
            paiAtual->esq = novo;
        } else {
            paiAtual->dir = novo;
        }

        splay(novo);
        totalElementos++;
    }

    bool buscar(int chave) {
        if (!raiz) return false;

        No* atual = raiz;
        No* ultimo = nullptr;

        while (atual) {
            ultimo = atual;
            if (chave < atual->chave) {
                atual = atual->esq;
            } else if (chave > atual->chave) {
                atual = atual->dir;
            } else {
                splay(atual);
                return true;
            }
        }

        // Se nao encontrou, traz o ultimo no visitado para a raiz
        if (ultimo) {
            splay(ultimo);
        }
        return false;
    }

    bool remover(int chave) {
        if (!buscar(chave)) return false; // buscar ja traz o no para a raiz se existir

        No* alvo = raiz; // Agora raiz->chave == chave
        No* subEsq = alvo->esq;
        No* subDir = alvo->dir;

        if (subEsq) subEsq->pai = nullptr;
        if (subDir) subDir->pai = nullptr;

        alvo->esq = nullptr;
        alvo->dir = nullptr;
        delete alvo;
        totalElementos--;

        if (!subEsq) {
            raiz = subDir;
        } else {
            // Encontra o maximo da subarvore esquerda
            No* maxEsq = subEsq;
            while (maxEsq->dir) {
                maxEsq = maxEsq->dir;
            }
            // Splay do maximo na subarvore esquerda
            raiz = subEsq;
            splay(maxEsq); // maxEsq agora e a nova raiz e nao possui filho direito!
            raiz->dir = subDir;
            if (subDir) subDir->pai = raiz;
        }

        return true;
    }

    size_t tamanho() const { return totalElementos; }
    bool vazia() const { return totalElementos == 0; }
    size_t altura() const { return calcularAlturaIterativo(raiz); }
    size_t rotacoes() const { return contagemRotacoes; }
    void zerarRotacoes() { contagemRotacoes = 0; }

    int getRaizChave() const {
        if (!raiz) throw std::runtime_error("Arvore vazia");
        return raiz->chave;
    }

    std::vector<int> listarEmOrdem() const {
        std::vector<int> res;
        emOrdem(raiz, res);
        return res;
    }
};

#endif // SPLAY_HPP
