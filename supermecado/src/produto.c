#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "produto.h"

float produto_arredondar_preco(double valor)
{
    return (float)(round(valor * 100.0) / 100.0);
}

bool produto_preco_valido(float preco)
{
    return isfinite(preco) && preco >= PRECO_MINIMO && preco <= PRECO_MAXIMO;
}

float produto_converter_preco(const char *texto)
{
    char copia[TAM_PRECO + 1];

    strncpy(copia, texto, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    for (int i = 0; copia[i] != '\0'; i++)
        if (copia[i] == ',')
            copia[i] = '.';

    return produto_arredondar_preco(atof(copia));
}
