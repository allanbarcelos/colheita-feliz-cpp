#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "GameState.h"
#include "Animal.h"
#include "Decoracao.h"
#include "Missoes.h"

using json = nlohmann::json;

inline std::string caminhoSavefile()
{
    char *base = SDL_GetPrefPath("FazendaDosSonhos", "FazendaDosSonhos");
    if (!base) return "save.json";
    std::string caminho = std::string(base) + "save.json";
    SDL_free(base);
    return caminho;
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Canteiro,
    coluna, linha, estado, tipoCrop, estagioCrop,
    timestampPlantio, temporadaAtual,
    saude, seca, praga, ultimoSorteioEventoMs)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Animal,
    tipo, x, y, direcao, targetX, targetY, timestampProximoTarget,
    timestampUltimaProducao, produtoPronto, comFome, timestampUltimaRefeicao)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Cachorro,
    raca, x, y, direcao, targetX, targetY, timestampProximoTarget, emMovimento)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DecoracaoColocada, idItem, x, y, rotacao)

inline void to_json(json &j, const Missao &m)
{
    j = json{
        {"tipo", m.tipo}, {"alvo", m.alvo}, {"progresso", m.progresso},
        {"concluida", m.concluida}, {"coletada", m.coletada},
        {"recompensaOuro", m.recompensaOuro}, {"recompensaVerdes", m.recompensaVerdes},
        {"recompensaXp", m.recompensaXp}, {"descricao", std::string(m.descricao)}
    };
}

inline void from_json(const json &j, Missao &m)
{
    m.tipo = j.value("tipo", 0);
    m.alvo = j.value("alvo", 0);
    m.progresso = j.value("progresso", 0);
    m.concluida = j.value("concluida", false);
    m.coletada = j.value("coletada", false);
    m.recompensaOuro = j.value("recompensaOuro", 0);
    m.recompensaVerdes = j.value("recompensaVerdes", 0);
    m.recompensaXp = j.value("recompensaXp", 0);
    std::string desc = j.value("descricao", std::string(""));
    snprintf(m.descricao, sizeof(m.descricao), "%s", desc.c_str());
}

inline bool salvarJogo(const GameState &s)
{
    json j;
    j["versao"] = 1;

    j["player"]["xp"] = s.xp;
    j["player"]["ouro"] = s.ouro;
    j["player"]["moedasVerdes"] = s.moedasVerdes;
    j["player"]["popularidade"] = s.popularidade;
    j["player"]["genero"] = s.generoJogador;
    j["player"]["canteirosComprados"] = s.canteirosComprados;
    j["player"]["colheitas"] = s.colheitas;

    j["tempoJogoMs"] = s.tempoJogoMs;

    j["canteiros"] = std::vector<Canteiro>(s.canteiros, s.canteiros + GRID_COLUNAS * GRID_LINHAS);

    j["inventarioSementes"] = std::vector<int>(s.inventarioSementes, s.inventarioSementes + TOTAL_CROPS);
    j["inventarioColhidos"] = std::vector<int>(s.inventarioColhidos, s.inventarioColhidos + TOTAL_CROPS);
    j["inventarioDecoracoes"] = std::vector<int>(s.inventarioDecoracoes, s.inventarioDecoracoes + TOTAL_DECORACOES);
    j["valorDeposito"] = s.valorDeposito;
    j["inventarioRacao"] = s.inventarioRacao;
    j["inventarioProdutos"] = std::vector<int>(s.inventarioProdutos, s.inventarioProdutos + 3);

    j["decoracoesColocadas"] = std::vector<DecoracaoColocada>(s.decoracoesColocadas,
                                                              s.decoracoesColocadas + s.totalDecoracoesColocadas);

    j["animais"] = std::vector<Animal>(s.animais, s.animais + 3);
    j["cachorro"] = s.cachorro;

    j["missoes"]["diarias"] = std::vector<Missao>(s.missoesDiarias, s.missoesDiarias + 3);
    j["missoes"]["ultimoReset"] = s.timestampUltimoResetMissoes;

    j["recompensa"]["ultimoDia"] = s.ultimoDiaRecompensa;
    j["recompensa"]["disponivel"] = s.recompensaDisponivel;

    std::string caminho = caminhoSavefile();
    std::ofstream out(caminho);
    if (!out)
    {
        std::cout << "[save] erro ao abrir " << caminho << "\n";
        return false;
    }
    out << j.dump(2);
    out.close();
    std::cout << "[save] salvo em " << caminho << "\n";
    return true;
}

inline bool saveExiste()
{
    std::ifstream in(caminhoSavefile());
    return static_cast<bool>(in);
}

inline bool carregarJogo(GameState &s)
{
    std::string caminho = caminhoSavefile();
    std::ifstream in(caminho);
    if (!in) { std::cout << "[save] arquivo nao existe: " << caminho << "\n"; return false; }

    json j;
    try { in >> j; }
    catch (json::parse_error &e)
    {
        std::cout << "[save] JSON corrompido: " << e.what() << "\n";
        return false;
    }

    if (j.value("versao", 0) != 1)
    {
        std::cout << "[save] versao incompativel\n";
        return false;
    }

    s.xp = j["player"].value("xp", 0);
    s.ouro = j["player"].value("ouro", 100);
    s.moedasVerdes = j["player"].value("moedasVerdes", 0);
    s.popularidade = j["player"].value("popularidade", 0);
    s.generoJogador = j["player"].value("genero", 0);
    s.canteirosComprados = j["player"].value("canteirosComprados", 0);
    s.colheitas = j["player"].value("colheitas", 0);

    s.tempoJogoMs = j.value("tempoJogoMs", 0u);

    if (j.contains("canteiros"))
    {
        auto vec = j["canteiros"].get<std::vector<Canteiro>>();
        for (size_t i = 0; i < vec.size() && i < (size_t)(GRID_COLUNAS * GRID_LINHAS); i++)
            s.canteiros[i] = vec[i];
    }

    auto carregaArray = [&j](const char *key, int *destino, int max) {
        if (!j.contains(key)) return;
        auto vec = j[key].get<std::vector<int>>();
        for (size_t i = 0; i < vec.size() && i < (size_t)max; i++)
            destino[i] = vec[i];
    };
    carregaArray("inventarioSementes", s.inventarioSementes, TOTAL_CROPS);
    carregaArray("inventarioColhidos", s.inventarioColhidos, TOTAL_CROPS);
    carregaArray("inventarioDecoracoes", s.inventarioDecoracoes, TOTAL_DECORACOES);
    carregaArray("inventarioProdutos", s.inventarioProdutos, 3);

    s.valorDeposito = j.value("valorDeposito", 0);
    s.inventarioRacao = j.value("inventarioRacao", RACAO_INICIAL);

    if (j.contains("decoracoesColocadas"))
    {
        auto vec = j["decoracoesColocadas"].get<std::vector<DecoracaoColocada>>();
        s.totalDecoracoesColocadas = static_cast<int>(std::min(vec.size(), (size_t)MAX_DECORACOES_COLOCADAS));
        for (int i = 0; i < s.totalDecoracoesColocadas; i++)
            s.decoracoesColocadas[i] = vec[i];
    }

    if (j.contains("animais"))
    {
        auto vec = j["animais"].get<std::vector<Animal>>();
        for (size_t i = 0; i < vec.size() && i < 3; i++)
            s.animais[i] = vec[i];
    }
    if (j.contains("cachorro")) s.cachorro = j["cachorro"].get<Cachorro>();

    if (j.contains("missoes"))
    {
        if (j["missoes"].contains("diarias"))
        {
            auto vec = j["missoes"]["diarias"].get<std::vector<Missao>>();
            for (size_t i = 0; i < vec.size() && i < 3; i++)
                s.missoesDiarias[i] = vec[i];
        }
        s.timestampUltimoResetMissoes = j["missoes"].value("ultimoReset", 0u);
    }

    if (j.contains("recompensa"))
    {
        s.ultimoDiaRecompensa = j["recompensa"].value("ultimoDia", -1);
        s.recompensaDisponivel = j["recompensa"].value("disponivel", true);
    }

    std::cout << "[save] carregado de " << caminho << "\n";
    return true;
}
