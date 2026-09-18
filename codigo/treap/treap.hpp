#ifndef TREAP_HPP
#define TREAP_HPP

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <climits>

/*
  A struct NoTreap define o no da arvore Treap (arvore + heap).
  Nela guardamos a chave para a busca binaria e uma prioridade para manter o heap.
*/
struct NoTreap {
    int chave; // Chave de busca (regra de BST: menor a esquerda, maior a direita)
    int prioridade; // Prioridade do heap (regra de Min-Heap: menor prioridade no topo)
    NoTreap* esq = nullptr; // Ponteiro para a subarvore esquerda
    NoTreap* dir = nullptr; // Ponteiro para a subarvore direita
};

// Funcao para criar e alocar um novo no na memoria
NoTreap* criarNoTreap(int chave, int prioridade) {
    NoTreap* novo = new NoTreap();
    novo->chave = chave;
    novo->prioridade = prioridade;
    novo->esq = nullptr;
    novo->dir = nullptr;
    return novo;
}

// Rotacao simples para a direita para ajustar as prioridades do heap
NoTreap* rotacionarDireitaTreap(NoTreap* y) {
    NoTreap* x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    return x;
}

// Rotacao simples para a esquerda para ajustar as prioridades do heap
NoTreap* rotacionarEsquerdaTreap(NoTreap* x) {
    NoTreap* y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    return y;
}

// Inserção recursiva na Treap respeitando BST nas chaves e Min-Heap nas prioridades
NoTreap* inserirTreap(NoTreap* no, int chave, int prioridade, bool* inserido = nullptr) {
    if (no == nullptr) {
        if (inserido != nullptr) *inserido = true;
        return criarNoTreap(chave, prioridade);
    }

    if (chave < no->chave) {
        no->esq = inserirTreap(no->esq, chave, prioridade, inserido);
        // Se o filho esquerdo tiver prioridade menor (mais urgente no Min-Heap), rotaciona à direita
        if (no->esq->prioridade < no->prioridade) {
            no = rotacionarDireitaTreap(no);
        }
    } else if (chave > no->chave) {
        no->dir = inserirTreap(no->dir, chave, prioridade, inserido);
        // Se o filho direito tiver prioridade menor, rotaciona à esquerda
        if (no->dir->prioridade < no->prioridade) {
            no = rotacionarEsquerdaTreap(no);
        }
    } else {
        // Chave duplicada: não insere
        if (inserido != nullptr) *inserido = false;
    }
    return no;
}

// Busca iterativa de uma chave na Treap
bool buscarTreap(NoTreap* raiz, int chave) {
    NoTreap* atual = raiz;
    while (atual != nullptr) {
        if (chave < atual->chave) {
            atual = atual->esq;
        } else if (chave > atual->chave) {
            atual = atual->dir;
        } else {
            return true;
        }
    }
    return false;
}

// Remoção recursiva descendo o nó via rotações até virar folha
NoTreap* removerTreap(NoTreap* no, int chave, bool* removido = nullptr) {
    if (no == nullptr) return nullptr;

    if (chave < no->chave) {
        no->esq = removerTreap(no->esq, chave, removido);
    } else if (chave > no->chave) {
        no->dir = removerTreap(no->dir, chave, removido);
    } else {
        // Encontrou o nó a remover
        if (removido != nullptr) *removido = true;

        if (no->esq == nullptr && no->dir == nullptr) {
            delete no;
            return nullptr;
        } else if (no->esq == nullptr) {
            NoTreap* temp = no->dir;
            delete no;
            return temp;
        } else if (no->dir == nullptr) {
            NoTreap* temp = no->esq;
            delete no;
            return temp;
        } else {
            // Ambos os filhos existem: rotaciona com o filho de menor prioridade
            if (no->esq->prioridade < no->dir->prioridade) {
                no = rotacionarDireitaTreap(no);
                no->dir = removerTreap(no->dir, chave, removido);
            } else {
                no = rotacionarEsquerdaTreap(no);
                no->esq = removerTreap(no->esq, chave, removido);
            }
        }
    }
    return no;
}

// Divisão (Split): divide a Treap em t1 (<= chave) e t2 (> chave)
void splitTreap(NoTreap* t, int chave, NoTreap*& t1, NoTreap*& t2) {
    if (t == nullptr) {
        t1 = t2 = nullptr;
    } else if (t->chave <= chave) {
        splitTreap(t->dir, chave, t->dir, t2);
        t1 = t;
    } else {
        splitTreap(t->esq, chave, t1, t->esq);
        t2 = t;
    }
}

// Fusão (Merge): combina t1 e t2 assumindo que todas as chaves de t1 < t2
void mergeTreap(NoTreap*& t, NoTreap* t1, NoTreap* t2) {
    if (t1 == nullptr || t2 == nullptr) {
        t = (t1 != nullptr) ? t1 : t2;
    } else if (t1->prioridade < t2->prioridade) {
        mergeTreap(t1->dir, t1->dir, t2);
        t = t1;
    } else {
        mergeTreap(t2->esq, t1, t2->esq);
        t = t2;
    }
}

// Valida formalmente as duas invariantes simultaneamente (BST e Min-Heap)
bool verificarInvariantesTreap(NoTreap* no, int minChave = INT_MIN, int maxChave = INT_MAX, int paiPrioridade = INT_MIN) {
    if (no == nullptr) return true;

    // 1. Invariante de BST nas chaves
    if (no->chave <= minChave || no->chave >= maxChave) return false;

    // 2. Invariante de Min-Heap nas prioridades
    if (no->prioridade < paiPrioridade) return false;

    return verificarInvariantesTreap(no->esq, minChave, no->chave, no->prioridade) &&
           verificarInvariantesTreap(no->dir, no->chave, maxChave, no->prioridade);
}

// Calcula a altura da Treap recursivamente
int alturaTreap(NoTreap* raiz) {
    if (raiz == nullptr) return 0;
    return 1 + std::max(alturaTreap(raiz->esq), alturaTreap(raiz->dir));
}

// Conta o número total de nós alocados
size_t contarNosTreap(NoTreap* raiz) {
    if (raiz == nullptr) return 0;
    return 1 + contarNosTreap(raiz->esq) + contarNosTreap(raiz->dir);
}

// Caminhamento em-ordem (in-order)
void listarEmOrdemTreap(NoTreap* raiz, std::vector<int>& resultado) {
    if (raiz == nullptr) return;
    listarEmOrdemTreap(raiz->esq, resultado);
    resultado.push_back(raiz->chave);
    listarEmOrdemTreap(raiz->dir, resultado);
}

// Libera recursivamente toda a memória alocada
void liberarTreap(NoTreap* raiz) {
    if (raiz != nullptr) {
        liberarTreap(raiz->esq);
        liberarTreap(raiz->dir);
        delete raiz;
    }
}

#endif // TREAP_HPP
