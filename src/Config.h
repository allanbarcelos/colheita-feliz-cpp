#pragma once

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

struct ConfigRuntime
{
    int offsetX = 500;
    int offsetY = 420;

    int casaX = 580, casaY = 30, casaW = 200, casaH = 200;
    int casinhaX = 760, casinhaY = 180, casinhaW = 80, casinhaH = 80;
};

inline ConfigRuntime g_config;

inline void aplicarChaveConfig(const std::string &chave, int valor)
{
    if (chave == "offset_x")        g_config.offsetX = valor;
    else if (chave == "offset_y")   g_config.offsetY = valor;
    else if (chave == "casa_x")     g_config.casaX = valor;
    else if (chave == "casa_y")     g_config.casaY = valor;
    else if (chave == "casa_w")     g_config.casaW = valor;
    else if (chave == "casa_h")     g_config.casaH = valor;
    else if (chave == "casinha_x")  g_config.casinhaX = valor;
    else if (chave == "casinha_y")  g_config.casinhaY = valor;
    else if (chave == "casinha_w")  g_config.casinhaW = valor;
    else if (chave == "casinha_h")  g_config.casinhaH = valor;
}

inline bool carregarConfig(const char *caminho)
{
    std::ifstream f(caminho);
    if (!f)
    {
        std::cout << "[config] falhou ao abrir " << caminho << std::endl;
        return false;
    }

    std::string linha;
    int aplicadas = 0;

    while (std::getline(f, linha))
    {
        size_t i = 0;
        while (i < linha.size() && std::isspace(static_cast<unsigned char>(linha[i])))
            i++;
        if (i >= linha.size())
            continue;
        if (linha[i] == '#' || linha[i] == ';')
            continue;

        size_t eq = linha.find('=', i);
        if (eq == std::string::npos)
            continue;

        std::string chave = linha.substr(i, eq - i);
        std::string valor = linha.substr(eq + 1);

        while (!chave.empty() && std::isspace(static_cast<unsigned char>(chave.back())))
            chave.pop_back();

        size_t vs = 0;
        while (vs < valor.size() && std::isspace(static_cast<unsigned char>(valor[vs])))
            vs++;
        valor = valor.substr(vs);
        while (!valor.empty() && std::isspace(static_cast<unsigned char>(valor.back())))
            valor.pop_back();

        if (chave.empty() || valor.empty())
            continue;

        try
        {
            int iv = std::stoi(valor);
            aplicarChaveConfig(chave, iv);
            aplicadas++;
        }
        catch (...)
        {
            continue;
        }
    }

    std::cout << "[config] " << aplicadas << " chaves de " << caminho << std::endl;
    return true;
}
