#ifndef PATRICIA_HPP
#define PATRICIA_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>

class Patricia {
public:
    struct No;

    struct Aresta {
        std::string rotulo;
        No* destino = nullptr;
    };

    struct No {
        bool terminal = false;
        std::unordered_map<char, Aresta> filhos;

        ~No() {
            for (auto& [c, a] : filhos) {
                delete a.destino;
            }
        }
    };

private:
    No* raiz = nullptr;
    size_t totalChaves = 0;

    static size_t calcularLCP(const std::string& a, const std::string& b) {
        size_t p = 0;
        while (p < a.size() && p < b.size() && a[p] == b[p]) {
            p++;
        }
        return p;
    }

    void inserirRecursivo(No* no, const std::string& resto) {
        if (resto.empty()) {
            if (!no->terminal) {
                no->terminal = true;
                totalChaves++;
            }
            return;
        }

        char c = resto[0];
        auto it = no->filhos.find(c);
        if (it == no->filhos.end()) {
            no->filhos[c] = {resto, new No{true, {}}};
            totalChaves++;
            return;
        }

        Aresta& e = it->second;
        size_t p = calcularLCP(resto, e.rotulo);

        if (p == e.rotulo.size()) {
            inserirRecursivo(e.destino, resto.substr(p));
        } else {
            // Cisao da aresta existente
            No* meio = new No{false, {}};
            std::string rotuloComum = e.rotulo.substr(0, p);
            std::string sufixoAntigo = e.rotulo.substr(p);

            meio->filhos[sufixoAntigo[0]] = {sufixoAntigo, e.destino};
            e = {rotuloComum, meio};

            if (p == resto.size()) {
                meio->terminal = true;
                totalChaves++;
            } else {
                std::string sufixoNovo = resto.substr(p);
                meio->filhos[sufixoNovo[0]] = {sufixoNovo, new No{true, {}}};
                totalChaves++;
            }
        }
    }

    bool removerRecursivo(No* no, const std::string& resto, bool& sucesso) {
        if (!no) return false;

        if (resto.empty()) {
            if (!no->terminal) return false;
            no->terminal = false;
            sucesso = true;
            totalChaves--;
            return no->filhos.empty();
        }

        char c = resto[0];
        auto it = no->filhos.find(c);
        if (it == no->filhos.end()) return false;

        Aresta& e = it->second;
        size_t p = calcularLCP(resto, e.rotulo);

        if (p != e.rotulo.size()) {
            return false; // Prefixo diverge antes do fim da aresta
        }

        bool deveDeletarDestino = removerRecursivo(e.destino, resto.substr(p), sucesso);

        if (deveDeletarDestino) {
            delete e.destino;
            no->filhos.erase(it);
        } else if (e.destino && !e.destino->terminal && e.destino->filhos.size() == 1) {
            // Fusao de arestas para manter a propriedade compacta
            auto filhoUnicoIt = e.destino->filhos.begin();
            Aresta subAresta = filhoUnicoIt->second;
            e.rotulo += subAresta.rotulo;
            No* noDescendente = subAresta.destino;

            e.destino->filhos.clear();
            delete e.destino;
            e.destino = noDescendente;
        }

        return !no->terminal && no->filhos.empty();
    }

    void coletarPalavras(No* atual, std::string prefixo, std::vector<std::string>& resultado) const {
        if (!atual) return;
        if (atual->terminal) {
            resultado.push_back(prefixo);
        }
        for (const auto& [c, aresta] : atual->filhos) {
            coletarPalavras(aresta.destino, prefixo + aresta.rotulo, resultado);
        }
    }

    size_t contarNosRecursivo(No* atual) const {
        if (!atual) return 0;
        size_t cont = 1;
        for (const auto& [c, aresta] : atual->filhos) {
            cont += contarNosRecursivo(aresta.destino);
        }
        return cont;
    }

public:
    Patricia() : raiz(new No{false, {}}) {}

    ~Patricia() {
        delete raiz;
    }

    Patricia(const Patricia&) = delete;
    Patricia& operator=(const Patricia&) = delete;
    Patricia(Patricia&&) noexcept = default;
    Patricia& operator=(Patricia&&) noexcept = default;

    void inserir(const std::string& chave) {
        inserirRecursivo(raiz, chave);
    }

    bool buscar(const std::string& chave) const {
        if (chave.empty()) return raiz->terminal;
        No* atual = raiz;
        std::string resto = chave;

        while (!resto.empty()) {
            char c = resto[0];
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return false;

            const Aresta& e = it->second;
            size_t p = calcularLCP(resto, e.rotulo);

            if (p < e.rotulo.size()) {
                return false; // Chave nao cobre o rotulo inteiro da aresta
            }
            resto = resto.substr(p);
            atual = e.destino;
        }
        return atual && atual->terminal;
    }

    bool comecaCom(const std::string& prefixo) const {
        if (prefixo.empty()) return true;
        No* atual = raiz;
        std::string resto = prefixo;

        while (!resto.empty()) {
            char c = resto[0];
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return false;

            const Aresta& e = it->second;
            size_t p = calcularLCP(resto, e.rotulo);

            if (p == resto.size()) {
                return true; // Prefixo foi completamente consumido
            }
            if (p < e.rotulo.size()) {
                return false; // Divergiu no meio do rotulo da aresta
            }
            resto = resto.substr(p);
            atual = e.destino;
        }
        return true;
    }

    bool remover(const std::string& chave) {
        if (chave.empty()) {
            if (raiz->terminal) {
                raiz->terminal = false;
                totalChaves--;
                return true;
            }
            return false;
        }
        bool sucesso = false;
        removerRecursivo(raiz, chave, sucesso);
        return sucesso;
    }

    std::vector<std::string> autocompletar(const std::string& prefixo) const {
        std::vector<std::string> resultado;
        if (prefixo.empty()) {
            coletarPalavras(raiz, "", resultado);
            return resultado;
        }

        No* atual = raiz;
        std::string resto = prefixo;
        std::string caminhoAcumulado = "";

        while (!resto.empty()) {
            char c = resto[0];
            auto it = atual->filhos.find(c);
            if (it == atual->filhos.end()) return resultado;

            const Aresta& e = it->second;
            size_t p = calcularLCP(resto, e.rotulo);

            if (p == resto.size()) {
                caminhoAcumulado += e.rotulo;
                coletarPalavras(e.destino, caminhoAcumulado, resultado);
                return resultado;
            }
            if (p < e.rotulo.size()) {
                return resultado; // Nenhuma chave compativel
            }
            caminhoAcumulado += e.rotulo;
            resto = resto.substr(p);
            atual = e.destino;
        }

        coletarPalavras(atual, caminhoAcumulado, resultado);
        return resultado;
    }

    size_t tamanho() const { return totalChaves; }
    bool vazia() const { return totalChaves == 0; }
    size_t contarNos() const { return contarNosRecursivo(raiz); }
};

#endif // PATRICIA_HPP
