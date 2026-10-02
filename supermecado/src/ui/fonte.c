#include <stdio.h>
#include "config.h"
#include "ui/fonte.h"

#define TAM_MAX_FONTE  64

/* ASCII + Latin-1: cobre os acentos do português. */
#define PRIMEIRO_CODEPOINT  32
#define ULTIMO_CODEPOINT    255

static const char *ARQUIVOS[TOTAL_PESOS] = {
    "Inter-Regular.ttf", "Inter-SemiBold.ttf"
};

static char caminhos[TOTAL_PESOS][TAM_CAMINHO];
static bool disponivel;

static Font cache[TOTAL_PESOS][TAM_MAX_FONTE + 1];
static bool carregada[TOTAL_PESOS][TAM_MAX_FONTE + 1];

static bool localizar(const char *arquivo, char destino[TAM_CAMINHO])
{
    /* relativo ao executável (bin/) e, como alternativa, ao diretório atual */
    const char *bases[] = { GetApplicationDirectory(), "" };
    const char *prefixos[] = { "../assets/fonts/", "assets/fonts/" };

    for (int b = 0; b < 2; b++) {
        for (int p = 0; p < 2; p++) {
            snprintf(destino, TAM_CAMINHO, "%s%s%s", bases[b], prefixos[p], arquivo);
            if (FileExists(destino)) return true;
        }
    }
    return false;
}

bool fonte_carregar(void)
{
    disponivel = true;

    for (int i = 0; i < TOTAL_PESOS; i++) {
        if (!localizar(ARQUIVOS[i], caminhos[i])) {
            TraceLog(LOG_WARNING, "FONTE: %s nao encontrada, usando fonte padrao", ARQUIVOS[i]);
            disponivel = false;
        }
    }
    return disponivel;
}

Font fonte_obter(Peso peso, float tamanho)
{
    int t = (int)(tamanho + 0.5f);

    if (!disponivel) return GetFontDefault();
    if (t < 1) t = 1;
    if (t > TAM_MAX_FONTE) t = TAM_MAX_FONTE;

    if (!carregada[peso][t]) {
        int codepoints[ULTIMO_CODEPOINT - PRIMEIRO_CODEPOINT + 1];
        int total = 0;

        for (int c = PRIMEIRO_CODEPOINT; c <= ULTIMO_CODEPOINT; c++)
            codepoints[total++] = c;

        /* silencia o aviso da raylib sobre glifos mais altos que o tamanho (ex.: '|') */
        SetTraceLogLevel(LOG_ERROR);
        cache[peso][t] = LoadFontEx(caminhos[peso], t, codepoints, total);
        SetTraceLogLevel(LOG_INFO);

        SetTextureFilter(cache[peso][t].texture, TEXTURE_FILTER_BILINEAR);
        carregada[peso][t] = true;
    }
    return cache[peso][t];
}

void fonte_descarregar(void)
{
    for (int p = 0; p < TOTAL_PESOS; p++) {
        for (int t = 0; t <= TAM_MAX_FONTE; t++) {
            if (carregada[p][t]) {
                UnloadFont(cache[p][t]);
                carregada[p][t] = false;
            }
        }
    }
}
