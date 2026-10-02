#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "config.h"
#include "entrada.h"

static bool eh_continuacao_utf8(char c)
{
    return ((unsigned char)c & 0xC0) == 0x80;
}

void processar_digitacao(char texto_campo[], int limite, TipoEntrada tipo)
{
    int tecla;

    while ((tecla = GetCharPressed()) > 0) {
        int tamanho = (int)strlen(texto_campo);
        bool permitido = false;

        switch (tipo) {
            case ENTRADA_TEXTO:
                /* ASCII imprimível + letras acentuadas (Latin-1) */
                permitido = (tecla >= 32 && tecla <= 126) || (tecla >= 160 && tecla <= 255);
                break;
            case ENTRADA_INTEIRO:
                permitido = (tecla >= '0' && tecla <= '9');
                break;
            case ENTRADA_DECIMAL:
                permitido = (tecla >= '0' && tecla <= '9') || tecla == '.' || tecla == ',';
                break;
        }

        if (permitido) {
            int bytes = 0;
            const char *utf8 = CodepointToUTF8(tecla, &bytes);

            if (tamanho + bytes > limite - 1) continue;

            memcpy(texto_campo + tamanho, utf8, bytes);
            texto_campo[tamanho + bytes] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        int tamanho = (int)strlen(texto_campo);

        /* remove o caractere inteiro, mesmo que ocupe vários bytes */
        while (tamanho > 0 && eh_continuacao_utf8(texto_campo[tamanho - 1]))
            tamanho--;
        if (tamanho > 0) tamanho--;

        texto_campo[tamanho] = '\0';
    }
}

float converter_preco(const char *texto)
{
    char copia[TAM_PRECO + 1];

    strncpy(copia, texto, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    for (int i = 0; copia[i] != '\0'; i++)
        if (copia[i] == ',')
            copia[i] = '.';

    return (float)atof(copia);
}
