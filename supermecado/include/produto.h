#ifndef PRODUTO_H
#define PRODUTO_H

#include "config.h"

typedef struct {
    int   codigo;
    char  nome[TAM_NOME];
    float preco;
} Produto;

#endif
