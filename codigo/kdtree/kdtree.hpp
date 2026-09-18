#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

using PontoKD = std::vector<double>;

/*
  A struct NoKD define o no da KD-Tree.
  Em vez de apenas um int, aqui guardamos um vetor com as k coordenadas do ponto.
  Os ponteiros esq e dir apontam para os nos filhos divididos pelo eixo do nivel.
*/
struct NoKD {
    PontoKD pt; // Coordenadas do ponto geometrico
    NoKD* esq = nullptr; // Ponteiro para a subarvore esquerda
    NoKD* dir = nullptr; // Ponteiro para a subarvore direita
};

// Funcao auxiliar para alocar um novo no com o ponto informado
NoKD* criarNoKD(const PontoKD& pt) {
    NoKD* novo = new NoKD();
    novo->pt = pt;
    novo->esq = nullptr;
    novo->dir = nullptr;
    return novo;
}

// Calcula a distância euclidiana quadrada entre dois pontos (evita sqrt desnecessário)
double distanciaEuclidianaQuadradaKD(const PontoKD& a, const PontoKD& b) {
    double d = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        d += diff * diff;
    }
    return d;
}

// Inserção incremental recursiva alternando eixos
NoKD* inserirKD(NoKD* no, const PontoKD& p, size_t prof = 0) {
    if (no == nullptr) {
        return criarNoKD(p);
    }

    size_t kDim = p.size();
    size_t eixo = prof % kDim;

    if (p[eixo] < no->pt[eixo]) {
        no->esq = inserirKD(no->esq, p, prof + 1);
    } else {
        no->dir = inserirKD(no->dir, p, prof + 1);
    }
    return no;
}

// Busca exata de uma coordenada na KD-Tree
bool buscarExatoKD(NoKD* no, const PontoKD& p, size_t prof = 0) {
    if (no == nullptr) return false;

    if (no->pt == p) return true;

    size_t kDim = p.size();
    size_t eixo = prof % kDim;

    if (p[eixo] < no->pt[eixo]) {
        return buscarExatoKD(no->esq, p, prof + 1);
    } else {
        return buscarExatoKD(no->dir, p, prof + 1);
    }
}

// Busca em faixa retangular (Range Query)
void buscaIntervaloKD(NoKD* no, const PontoKD& minP, const PontoKD& maxP, size_t prof, std::vector<PontoKD>& res) {
    if (no == nullptr) return;

    size_t kDim = minP.size();

    // Verifica se o ponto atual está estritamente dentro da caixa delimitadora
    bool dentro = true;
    for (size_t i = 0; i < kDim; ++i) {
        if (no->pt[i] < minP[i] || no->pt[i] > maxP[i]) {
            dentro = false;
            break;
        }
    }
    if (dentro) {
        res.push_back(no->pt);
    }

    size_t eixo = prof % kDim;

    // Poda: só desce para a esquerda se a região puder conter pontos >= minP[eixo]
    if (minP[eixo] <= no->pt[eixo]) {
        buscaIntervaloKD(no->esq, minP, maxP, prof + 1, res);
    }
    // Poda: só desce para a direita se a região puder conter pontos <= maxP[eixo]
    if (maxP[eixo] >= no->pt[eixo]) {
        buscaIntervaloKD(no->dir, minP, maxP, prof + 1, res);
    }
}

// Algoritmo de busca do Vizinho Mais Próximo (1-NN) com poda geométrica
void kNNRecursivoKD(NoKD* no, const PontoKD& consulta, size_t prof, NoKD*& melhorNo, double& melhorDistQuadrada, size_t& nosVisitados) {
    if (no == nullptr) return;

    nosVisitados++;
    double d = distanciaEuclidianaQuadradaKD(consulta, no->pt);
    if (d < melhorDistQuadrada) {
        melhorDistQuadrada = d;
        melhorNo = no;
    }

    size_t kDim = consulta.size();
    size_t eixo = prof % kDim;
    double delta = consulta[eixo] - no->pt[eixo];

    NoKD* perto = (delta < 0) ? no->esq : no->dir;
    NoKD* longe = (delta < 0) ? no->dir : no->esq;

    // Primeiro busca no lado onde o ponto de consulta se localiza
    kNNRecursivoKD(perto, consulta, prof + 1, melhorNo, melhorDistQuadrada, nosVisitados);

    // Poda geométrica: só inspeciona o outro lado se o hiperplano divisor estiver
    // mais próximo do que o melhor candidato encontrado até agora
    if (delta * delta < melhorDistQuadrada) {
        kNNRecursivoKD(longe, consulta, prof + 1, melhorNo, melhorDistQuadrada, nosVisitados);
    }
}

// Função principal de busca de vizinho mais próximo (1-NN)
PontoKD vizinhoMaisProximoKD(NoKD* raiz, const PontoKD& consulta, size_t* nosVisitadosOut = nullptr) {
    if (raiz == nullptr) throw std::runtime_error("KD-Tree vazia");

    NoKD* melhorNo = nullptr;
    double melhorDist = std::numeric_limits<double>::infinity();
    size_t visitados = 0;

    kNNRecursivoKD(raiz, consulta, 0, melhorNo, melhorDist, visitados);

    if (nosVisitadosOut != nullptr) {
        *nosVisitadosOut = visitados;
    }
    return melhorNo->pt;
}

// Construção estática balanceada da KD-Tree usando a mediana em cada nível
NoKD* construirBalanceadaKD(std::vector<PontoKD>& pontos, size_t inicio, size_t fim, size_t prof = 0) {
    if (inicio >= fim) return nullptr;

    size_t kDim = pontos[0].size();
    size_t eixo = prof % kDim;
    size_t meio = inicio + (fim - inicio) / 2;

    // Particiona os pontos colocando a mediana na posição correta
    std::nth_element(
        pontos.begin() + inicio,
        pontos.begin() + meio,
        pontos.begin() + fim,
        [eixo](const PontoKD& a, const PontoKD& b) {
            return a[eixo] < b[eixo];
        }
    );

    NoKD* no = criarNoKD(pontos[meio]);
    no->esq = construirBalanceadaKD(pontos, inicio, meio, prof + 1);
    no->dir = construirBalanceadaKD(pontos, meio + 1, fim, prof + 1);
    return no;
}

// Calcula a altura da árvore recursivamente
int alturaKD(NoKD* no) {
    if (no == nullptr) return 0;
    return 1 + std::max(alturaKD(no->esq), alturaKD(no->dir));
}

// Conta o total de nós na KD-Tree
size_t contarNosKD(NoKD* no) {
    if (no == nullptr) return 0;
    return 1 + contarNosKD(no->esq) + contarNosKD(no->dir);
}

// Libera recursivamente toda a memória alocada para a KD-Tree
void liberarKD(NoKD* raiz) {
    if (raiz != nullptr) {
        liberarKD(raiz->esq);
        liberarKD(raiz->dir);
        delete raiz;
    }
}

#endif // KDTREE_HPP
