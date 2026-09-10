#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

class KDTree {
public:
    using Ponto = std::vector<double>;

    struct No {
        Ponto pt;
        No* esq = nullptr;
        No* dir = nullptr;

        No(Ponto p) : pt(p) {}

        ~No() {
            delete esq;
            delete dir;
        }
    };

private:
    No* raiz = nullptr;
    size_t kDim = 0;
    size_t totalPontos = 0;

    static double distanciaEuclidianaQuadrada(const Ponto& a, const Ponto& b) {
        double d = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            d += diff * diff;
        }
        return d;
    }

    No* construirRecursivo(std::vector<Ponto>& pontos, size_t inicio, size_t fim, size_t prof) {
        if (inicio >= fim) return nullptr;

        size_t eixo = prof % kDim;
        size_t meio = inicio + (fim - inicio) / 2;

        std::nth_element(
            pontos.begin() + inicio,
            pontos.begin() + meio,
            pontos.begin() + fim,
            [eixo](const Ponto& a, const Ponto& b) {
                return a[eixo] < b[eixo];
            }
        );

        No* no = new No(pontos[meio]);
        no->esq = construirRecursivo(pontos, inicio, meio, prof + 1);
        no->dir = construirRecursivo(pontos, meio + 1, fim, prof + 1);
        return no;
    }

    No* inserirRecursivo(No* no, const Ponto& p, size_t prof) {
        if (!no) {
            totalPontos++;
            return new No(p);
        }

        size_t eixo = prof % kDim;
        if (p[eixo] < no->pt[eixo]) {
            no->esq = inserirRecursivo(no->esq, p, prof + 1);
        } else {
            // Em caso de empate, segue para a subarvore direita
            no->dir = inserirRecursivo(no->dir, p, prof + 1);
        }
        return no;
    }

    bool buscarExatoRecursivo(No* no, const Ponto& p, size_t prof) const {
        if (!no) return false;

        if (no->pt == p) return true;

        size_t eixo = prof % kDim;
        if (p[eixo] < no->pt[eixo]) {
            return buscarExatoRecursivo(no->esq, p, prof + 1);
        } else {
            return buscarExatoRecursivo(no->dir, p, prof + 1);
        }
    }

    void buscaIntervaloRecursivo(No* no, const Ponto& minP, const Ponto& maxP, size_t prof, std::vector<Ponto>& res) const {
        if (!no) return;

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
        if (minP[eixo] <= no->pt[eixo]) {
            buscaIntervaloRecursivo(no->esq, minP, maxP, prof + 1, res);
        }
        if (maxP[eixo] >= no->pt[eixo]) {
            buscaIntervaloRecursivo(no->dir, minP, maxP, prof + 1, res);
        }
    }

    void kNNRecursivo(No* no, const Ponto& consulta, size_t prof, No*& melhorNo, double& melhorDistQuadrada, size_t& nosVisitados) const {
        if (!no) return;

        nosVisitados++;
        double d = distanciaEuclidianaQuadrada(consulta, no->pt);
        if (d < melhorDistQuadrada) {
            melhorDistQuadrada = d;
            melhorNo = no;
        }

        size_t eixo = prof % kDim;
        double delta = consulta[eixo] - no->pt[eixo];

        No* perto = (delta < 0) ? no->esq : no->dir;
        No* longe = (delta < 0) ? no->dir : no->esq;

        kNNRecursivo(perto, consulta, prof + 1, melhorNo, melhorDistQuadrada, nosVisitados);

        // Poda euclidiana pelo hiperplano ortogonal divisorio
        if (delta * delta < melhorDistQuadrada) {
            kNNRecursivo(longe, consulta, prof + 1, melhorNo, melhorDistQuadrada, nosVisitados);
        }
    }

    size_t calcularAltura(No* no) const {
        if (!no) return 0;
        return 1 + std::max(calcularAltura(no->esq), calcularAltura(no->dir));
    }

public:
    KDTree(size_t dimensao = 2) : kDim(dimensao) {}

    ~KDTree() {
        delete raiz;
    }

    KDTree(const KDTree&) = delete;
    KDTree& operator=(const KDTree&) = delete;

    KDTree(KDTree&& other) noexcept
        : raiz(other.raiz), kDim(other.kDim), totalPontos(other.totalPontos) {
        other.raiz = nullptr;
        other.totalPontos = 0;
    }

    KDTree& operator=(KDTree&& other) noexcept {
        if (this != &other) {
            delete raiz;
            raiz = other.raiz;
            kDim = other.kDim;
            totalPontos = other.totalPontos;
            other.raiz = nullptr;
            other.totalPontos = 0;
        }
        return *this;
    }

    void construirBalanceada(std::vector<Ponto> pontos) {
        delete raiz;
        raiz = nullptr;
        totalPontos = pontos.size();
        if (!pontos.empty()) {
            kDim = pontos[0].size();
            raiz = construirRecursivo(pontos, 0, pontos.size(), 0);
        }
    }

    void inserir(const Ponto& p) {
        if (kDim == 0) kDim = p.size();
        if (p.size() != kDim) {
            throw std::invalid_argument("Dimensao incompativel do ponto");
        }
        raiz = inserirRecursivo(raiz, p, 0);
    }

    bool buscarExato(const Ponto& p) const {
        if (p.size() != kDim) return false;
        return buscarExatoRecursivo(raiz, p, 0);
    }

    std::vector<Ponto> buscaIntervalo(const Ponto& minP, const Ponto& maxP) const {
        std::vector<Ponto> res;
        buscaIntervaloRecursivo(raiz, minP, maxP, 0, res);
        return res;
    }

    Ponto vizinhoMaisProximo(const Ponto& consulta, size_t* nosVisitadosOut = nullptr) const {
        if (!raiz) throw std::runtime_error("KD-Tree vazia");
        if (consulta.size() != kDim) throw std::invalid_argument("Dimensao incompativel");

        No* melhorNo = nullptr;
        double melhorDist = std::numeric_limits<double>::infinity();
        size_t visitados = 0;

        kNNRecursivo(raiz, consulta, 0, melhorNo, melhorDist, visitados);

        if (nosVisitadosOut) *nosVisitadosOut = visitados;
        return melhorNo->pt;
    }

    size_t tamanho() const { return totalPontos; }
    bool vazia() const { return totalPontos == 0; }
    size_t altura() const { return calcularAltura(raiz); }
    size_t dimensao() const { return kDim; }

    No* getRaiz() const { return raiz; }
};

#endif // KDTREE_HPP
