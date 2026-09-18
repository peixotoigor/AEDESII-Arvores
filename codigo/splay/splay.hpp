#ifndef SPLAY_HPP
#define SPLAY_HPP

#include <iostream>
#include <vector>
#include <algorithm>

/*
  A struct NoSplay cria o tipo de dado para representar o nó de uma Árvore Splay.
  Além dos ponteiros para os filhos da esquerda e da direita, guarda um ponteiro para
  o nó pai, essencial para conseguir subir o nó até a raiz através de rotações.
*/
struct NoSplay {
    int chave; // Armazena a chave ou valor do nó
    NoSplay* esq = nullptr; // Ponteiro para o filho esquerdo
    NoSplay* dir = nullptr; // Ponteiro para o filho direito
    NoSplay* pai = nullptr; // Ponteiro para o pai (permite subir até a raiz)

    /*
      NoSplay* esq, dir e pai guardam endereços de memória na heap.
      Quando um nó é criado sem conexões, todos os três iniciam como nullptr.
    */
};

/*
  Cria e aloca dinamicamente um novo nó da Splay na memória.
*/
NoSplay* criarNoSplay(int chave) {
    NoSplay* novo = new NoSplay();
    novo->chave = chave;
    novo->esq = nullptr;
    novo->dir = nullptr;
    novo->pai = nullptr;
    return novo;
}

/*
  Promove o nó x um nível acima, fazendo uma rotação simples sobre seu nó pai p.
  Atualiza os ponteiros dos filhos e do avô g.
*/
void promoverSplay(NoSplay*& raiz, NoSplay* x, size_t* contagemRotacoes = nullptr) {
    NoSplay* p = x->pai;
    if (p == nullptr) return;
    NoSplay* g = p->pai;

    if (contagemRotacoes != nullptr) {
        (*contagemRotacoes)++;
    }

    // Se x é filho esquerdo, rotação à direita
    if (x == p->esq) {
        p->esq = x->dir;
        if (x->dir != nullptr) x->dir->pai = p;
        x->dir = p;
    } else {
        // Se x é filho direito, rotação à esquerda
        p->dir = x->esq;
        if (x->esq != nullptr) x->esq->pai = p;
        x->esq = p;
    }

    p->pai = x;
    x->pai = g;

    // Atualiza o ponteiro do avô para apontar para x
    if (g != nullptr) {
        if (p == g->esq) g->esq = x;
        else g->dir = x;
    } else {
        raiz = x; // Se não tem avô, x virou a nova raiz da árvore
    }
}

/*
  Operação principal de Splay: sobe o nó x até a raiz através de rotações consecutivas.
  Combina os casos:
  - Zig: quando o pai de x é a raiz
  - Zig-Zig: x e seu pai estão no mesmo sentido (ambos esq ou ambos dir)
  - Zig-Zag: x e seu pai estão em sentidos alternados
*/
void splay(NoSplay*& raiz, NoSplay* x, size_t* contagemRotacoes = nullptr) {
    if (x == nullptr) return;

    while (x->pai != nullptr) {
        NoSplay* p = x->pai;
        NoSplay* g = p->pai;

        if (g == nullptr) {
            // Caso Zig: pai é a própria raiz, basta uma rotação
            promoverSplay(raiz, x, contagemRotacoes);
        } else if ((x == p->esq) == (p == g->esq)) {
            // Caso Zig-Zig: mesmo alinhamento. Primeiro rotaciona o pai, depois x
            promoverSplay(raiz, p, contagemRotacoes);
            promoverSplay(raiz, x, contagemRotacoes);
        } else {
            // Caso Zig-Zag: alinhamento alternado. Rotaciona x duas vezes
            promoverSplay(raiz, x, contagemRotacoes);
            promoverSplay(raiz, x, contagemRotacoes);
        }
    }
}

/*
  Insere uma nova chave na Árvore Splay.
  Insere na posição correta como em uma BST e em seguida faz splay do nó até a raiz.
*/
void inserirSplay(NoSplay*& raiz, int chave, size_t* contagemRotacoes = nullptr) {
    // Se a árvore estiver vazia, o novo nó vira a raiz
    if (raiz == nullptr) {
        raiz = criarNoSplay(chave);
        return;
    }

    NoSplay* atual = raiz;
    NoSplay* paiAtual = nullptr;

    while (atual != nullptr) {
        paiAtual = atual;
        if (chave < atual->chave) {
            atual = atual->esq;
        } else if (chave > atual->chave) {
            atual = atual->dir;
        } else {
            // Chave já existe: apenas traz o nó para a raiz
            splay(raiz, atual, contagemRotacoes);
            return;
        }
    }

    // Cria o novo nó e conecta com o pai
    NoSplay* novo = criarNoSplay(chave);
    novo->pai = paiAtual;
    if (chave < paiAtual->chave) {
        paiAtual->esq = novo;
    } else {
        paiAtual->dir = novo;
    }

    // Traz o nó recém-inserido para a raiz
    splay(raiz, novo, contagemRotacoes);
}

/*
  Busca uma chave na Splay.
  Se encontrar, traz o nó para a raiz. Se não encontrar, traz o último nó visitado.
*/
bool buscarSplay(NoSplay*& raiz, int chave, size_t* contagemRotacoes = nullptr) {
    if (raiz == nullptr) return false;

    NoSplay* atual = raiz;
    NoSplay* ultimo = nullptr;

    while (atual != nullptr) {
        ultimo = atual;
        if (chave < atual->chave) {
            atual = atual->esq;
        } else if (chave > atual->chave) {
            atual = atual->dir;
        } else {
            // Chave encontrada: sobe o nó até a raiz
            splay(raiz, atual, contagemRotacoes);
            return true;
        }
    }

    // Chave não encontrada: sobe o último nó acessado para aproximar da raiz
    if (ultimo != nullptr) {
        splay(raiz, ultimo, contagemRotacoes);
    }
    return false;
}

/*
  Remove uma chave da Árvore Splay.
  Primeiro busca a chave (trazendo-a para a raiz) e depois desconecta e junta as duas subárvores.
*/
bool removerSplay(NoSplay*& raiz, int chave, size_t* contagemRotacoes = nullptr) {
    if (!buscarSplay(raiz, chave, contagemRotacoes)) {
        return false; // Chave não existe na árvore
    }

    // Agora o nó a ser removido é a raiz
    NoSplay* alvo = raiz;
    NoSplay* subEsq = alvo->esq;
    NoSplay* subDir = alvo->dir;

    if (subEsq != nullptr) subEsq->pai = nullptr;
    if (subDir != nullptr) subDir->pai = nullptr;

    delete alvo;

    if (subEsq == nullptr) {
        raiz = subDir;
    } else {
        // Encontra o maior elemento da subárvore esquerda
        NoSplay* maxEsq = subEsq;
        while (maxEsq->dir != nullptr) {
            maxEsq = maxEsq->dir;
        }
        // Promove o maior elemento para ser a raiz da subárvore esquerda
        raiz = subEsq;
        splay(raiz, maxEsq, contagemRotacoes);
        // O filho direito da nova raiz fica livre para receber a subárvore direita
        raiz->dir = subDir;
        if (subDir != nullptr) subDir->pai = raiz;
    }

    return true;
}

/*
  Calcula a altura da árvore de forma iterativa com pilha para evitar estouro de memória.
*/
int alturaSplay(NoSplay* raiz) {
    if (raiz == nullptr) return 0;
    std::vector<std::pair<NoSplay*, int>> pilha;
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
  Conta o total de nós na árvore Splay.
*/
size_t contarNosSplay(NoSplay* raiz) {
    if (raiz == nullptr) return 0;
    size_t total = 0;
    std::vector<NoSplay*> pilha;
    pilha.reserve(128);
    pilha.push_back(raiz);
    while (!pilha.empty()) {
        NoSplay* no = pilha.back();
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
  Percorre a árvore em-ordem (esquerda, raiz, direita) salvando as chaves ordenadas.
*/
void listarEmOrdemSplay(NoSplay* raiz, std::vector<int>& resultado) {
    if (raiz == nullptr) return;
    listarEmOrdemSplay(raiz->esq, resultado);
    resultado.push_back(raiz->chave);
    listarEmOrdemSplay(raiz->dir, resultado);
}

/*
  Libera a memoria de todos os nos da arvore recursivamente em pos-ordem.
*/
void liberarSplay(NoSplay* raiz) {
    if (raiz != nullptr) {
        liberarSplay(raiz->esq);
        liberarSplay(raiz->dir);
        delete raiz;
    }
}

#endif // SPLAY_HPP
