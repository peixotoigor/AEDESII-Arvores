#ifndef BST_HPP
#define BST_HPP

#include <iostream>
#include <vector>

/*
  A struct NoBST define o no da arvore binaria de busca (BST).
  Uma struct agrupa variaveis em um unico tipo de dado.
*/
struct NoBST {
    int chave; // Armazena o valor inteiro
    NoBST* esq; // Ponteiro para o filho esquerdo
    NoBST* dir; // Ponteiro para o filho direito

    /*
      Construtor para inicializar o no com a chave passada
      e apontar os filhos para nulo.
    */
    NoBST(int c = 0) {
        chave = c;
        esq = nullptr;
        dir = nullptr;
    }
};

/*
  Funcao para criar e alocar um novo no na memoria.
*/
NoBST* criarNoBST(int chave) {
    return new NoBST(chave);
}

/*
  Insere uma nova chave na arvore binaria de busca.
  Se o valor for menor desce para a esquerda, se for maior desce para a direita.
*/
NoBST* inserirBST(NoBST* raiz, int chave) {
    // Se a raiz for nula, o novo no se torna a raiz
    if (raiz == nullptr) {
        return criarNoBST(chave);
    }

    NoBST* atual = raiz;
    while (true) {
        // Se a chave for menor que o valor do no atual, vai para a esquerda
        if (chave < atual->chave) {
            if (atual->esq == nullptr) {
                atual->esq = criarNoBST(chave);
                break;
            }
            atual = atual->esq;
        } else if (chave > atual->chave) {
            // Se a chave for maior que o valor do no atual, vai para a direita
            if (atual->dir == nullptr) {
                atual->dir = criarNoBST(chave);
                break;
            }
            atual = atual->dir;
        } else {
            // Chave duplicada: nao insere repetido
            break;
        }
    }
    return raiz;
}

/*
  Busca um valor na arvore binaria de busca.
  Retorna true se encontrar ou false se nao existir.
*/
bool buscarBST(NoBST* raiz, int chave) {
    NoBST* atual = raiz;
    while (atual != nullptr) {
        if (chave < atual->chave) {
            atual = atual->esq;
        } else if (chave > atual->chave) {
            atual = atual->dir;
        } else {
            return true; // Encontrou a chave procurada
        }
    }
    return false; // Nao encontrou
}

/*
  Calcula a altura da arvore.
*/
int alturaBST(NoBST* raiz) {
    if (raiz == nullptr) return 0;
    std::vector<std::pair<NoBST*, int>> pilha;
    pilha.reserve(128);
    pilha.push_back({raiz, 1});
    int maxAlt = 0;
    while (!pilha.empty()) {
        auto [no, alt] = pilha.back();
        pilha.pop_back();
        if (no != nullptr) {
            if (alt > maxAlt) maxAlt = alt;
            if (no->esq != nullptr) pilha.push_back({no->esq, alt + 1});
            if (no->dir != nullptr) pilha.push_back({no->dir, alt + 1});
        }
    }
    return maxAlt;
}

/*
  Conta a quantidade total de nos na arvore.
*/
size_t contarNosBST(NoBST* raiz) {
    if (raiz == nullptr) return 0;
    size_t total = 0;
    std::vector<NoBST*> pilha;
    pilha.reserve(128);
    pilha.push_back(raiz);
    while (!pilha.empty()) {
        NoBST* no = pilha.back();
        pilha.pop_back();
        if (no != nullptr) {
            total++;
            if (no->dir != nullptr) pilha.push_back(no->dir);
            if (no->esq != nullptr) pilha.push_back(no->esq);
        }
    }
    return total;
}

/*
  Libera toda a memoria alocada para os nos da arvore.
  Usa o percurso pos-ordem: primeiro libera os filhos da esquerda e da direita,
  e depois deleta o no raiz.
*/
void liberarBST(NoBST* raiz) {
    if (raiz != nullptr) {
        liberarBST(raiz->esq);
        liberarBST(raiz->dir);
        delete raiz;
    }
}

#endif // BST_HPP
