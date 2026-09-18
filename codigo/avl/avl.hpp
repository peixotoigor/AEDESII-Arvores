#ifndef AVL_HPP
#define AVL_HPP

#include <iostream>
#include <algorithm>
#include <vector>

/*
  A struct NoAVL define o no da arvore AVL.
  Alem da chave inteira e dos ponteiros esquerdo e direito,
  ela guarda a altura do no para calcular o fator de balanceamento.
*/
struct NoAVL {
    int chave; // Armazena o valor inteiro
    int altura; // Armazena a altura do no (inicia em 1)
    NoAVL* esq; // Ponteiro para o filho esquerdo
    NoAVL* dir; // Ponteiro para o filho direito

    /*
      Construtor para inicializar o no com a chave passada,
      altura 1 e filhos apontando para nulo.
    */
    NoAVL(int c = 0) {
        chave = c;
        altura = 1;
        esq = nullptr;
        dir = nullptr;
    }
};

/*
  Funcao para criar e alocar um novo no na memoria.
*/
NoAVL* criarNoAVL(int chave) {
    return new NoAVL(chave);
}

/*
  Retorna a altura de um no.
  Se o no for nulo (folha vazia), a altura considerada e 0.
*/
int obterAlturaAVL(NoAVL* no) {
    if (no == nullptr) return 0;
    return no->altura;
}

/*
  Calcula o fator de balanceamento: altura da direita menos altura da esquerda.
  Em uma arvore AVL valida, o fator deve ser -1, 0 ou +1.
*/
int fatorBalancoAVL(NoAVL* no) {
    if (no == nullptr) return 0;
    return obterAlturaAVL(no->dir) - obterAlturaAVL(no->esq);
}

/*
  Atualiza a altura do no com base na maior altura entre seus dois filhos mais 1.
*/
void atualizarAlturaAVL(NoAVL* no) {
    if (no != nullptr) {
        no->altura = 1 + std::max(obterAlturaAVL(no->esq), obterAlturaAVL(no->dir));
    }
}

/*
  Rotacao simples para a direita.
  Usada quando a subarvore esquerda fica mais pesada.
*/
NoAVL* rotacionarDireitaAVL(NoAVL* y) {
    NoAVL* x = y->esq;
    NoAVL* t2 = x->dir;

    // Ajusta os ponteiros
    x->dir = y;
    y->esq = t2;

    // Atualiza as alturas dos nos que mudaram de posicao
    atualizarAlturaAVL(y);
    atualizarAlturaAVL(x);

    // Retorna a nova raiz da subarvore
    return x;
}

/*
  Rotacao simples para a esquerda.
  Usada quando a subarvore direita fica mais pesada.
*/
NoAVL* rotacionarEsquerdaAVL(NoAVL* x) {
    NoAVL* y = x->dir;
    NoAVL* t2 = y->esq;

    // Ajusta os ponteiros
    y->esq = x;
    x->dir = t2;

    // Atualiza as alturas dos nos que mudaram de posicao
    atualizarAlturaAVL(x);
    atualizarAlturaAVL(y);

    // Retorna a nova raiz da subarvore
    return y;
}

/*
  Insere uma nova chave na arvore AVL e aplica rotacoes se desbalancear.
*/
NoAVL* inserirAVL(NoAVL* no, int chave) {
    // Se a posicao estiver vazia, cria o novo no
    if (no == nullptr) {
        return criarNoAVL(chave);
    }

    if (chave < no->chave) {
        no->esq = inserirAVL(no->esq, chave);
    } else if (chave > no->chave) {
        no->dir = inserirAVL(no->dir, chave);
    } else {
        // Chave duplicada: nao insere repetido
        return no;
    }

    // Atualiza a altura do no atual
    atualizarAlturaAVL(no);

    // Calcula o fator de balanceamento
    int fb = fatorBalancoAVL(no);

    // Caso Esquerda-Esquerda: rotacao simples a direita
    if (fb < -1 && chave < no->esq->chave) {
        return rotacionarDireitaAVL(no);
    }

    // Caso Direita-Direita: rotacao simples a esquerda
    if (fb > 1 && chave > no->dir->chave) {
        return rotacionarEsquerdaAVL(no);
    }

    // Caso Esquerda-Direita: rotacao dupla (esquerda depois direita)
    if (fb < -1 && chave > no->esq->chave) {
        no->esq = rotacionarEsquerdaAVL(no->esq);
        return rotacionarDireitaAVL(no);
    }

    // Caso Direita-Esquerda: rotacao dupla (direita depois esquerda)
    if (fb > 1 && chave < no->dir->chave) {
        no->dir = rotacionarDireitaAVL(no->dir);
        return rotacionarEsquerdaAVL(no);
    }

    return no;
}

/*
  Encontra o menor no de uma subarvore (o no mais a esquerda).
*/
NoAVL* buscarMenorNoAVL(NoAVL* no) {
    NoAVL* atual = no;
    while (atual != nullptr && atual->esq != nullptr) {
        atual = atual->esq;
    }
    return atual;
}

/*
  Remove uma chave da arvore AVL e restaura o balanceamento.
*/
NoAVL* removerAVL(NoAVL* no, int chave, bool* removido = nullptr) {
    if (no == nullptr) return nullptr;

    if (chave < no->chave) {
        no->esq = removerAVL(no->esq, chave, removido);
    } else if (chave > no->chave) {
        no->dir = removerAVL(no->dir, chave, removido);
    } else {
        // Encontrou o no a ser removido
        if (removido != nullptr) *removido = true;

        if (no->esq == nullptr) {
            NoAVL* temp = no->dir;
            delete no;
            no = temp;
        } else if (no->dir == nullptr) {
            NoAVL* temp = no->esq;
            delete no;
            no = temp;
        } else {
            // No com dois filhos: pega o sucessor (menor da direita)
            NoAVL* temp = buscarMenorNoAVL(no->dir);
            no->chave = temp->chave;
            no->dir = removerAVL(no->dir, temp->chave, nullptr);
        }
    }

    if (no == nullptr) return nullptr;

    atualizarAlturaAVL(no);
    int fb = fatorBalancoAVL(no);

    // Ajusta o balanceamento apos a remocao
    if (fb < -1 && fatorBalancoAVL(no->esq) <= 0) {
        return rotacionarDireitaAVL(no);
    }
    if (fb < -1 && fatorBalancoAVL(no->esq) > 0) {
        no->esq = rotacionarEsquerdaAVL(no->esq);
        return rotacionarDireitaAVL(no);
    }
    if (fb > 1 && fatorBalancoAVL(no->dir) >= 0) {
        return rotacionarEsquerdaAVL(no);
    }
    if (fb > 1 && fatorBalancoAVL(no->dir) < 0) {
        no->dir = rotacionarDireitaAVL(no->dir);
        return rotacionarEsquerdaAVL(no);
    }

    return no;
}

/*
  Busca um valor na arvore AVL.
*/
bool buscarAVL(NoAVL* raiz, int chave) {
    NoAVL* atual = raiz;
    while (atual != nullptr) {
        if (chave < atual->chave) {
            atual = atual->esq;
        } else if (chave > atual->chave) {
            atual = atual->dir;
        } else {
            return true; // Encontrou o valor
        }
    }
    return false; // Nao encontrou
}

/*
  Conta a quantidade total de nos na arvore AVL.
*/
size_t contarNosAVL(NoAVL* raiz) {
    if (raiz == nullptr) return 0;
    return 1 + contarNosAVL(raiz->esq) + contarNosAVL(raiz->dir);
}

/*
  Libera a memoria de todos os nos da arvore recursivamente em pos-ordem.
*/
void liberarAVL(NoAVL* raiz) {
    if (raiz != nullptr) {
        liberarAVL(raiz->esq);
        liberarAVL(raiz->dir);
        delete raiz;
    }
}

#endif // AVL_HPP
