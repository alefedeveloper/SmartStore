#ifndef ESTOQUE_H
#define ESTOQUE_H

#include <stdbool.h>
#include "produto.h"

typedef struct {
    Produto itens[MAX_PRODUTOS];
    int     quantidade;
} Estoque;

void estoque_adicionar(Estoque *e, int codigo, const char *nome, float preco);
void estoque_carregar_iniciais(Estoque *e);
bool estoque_codigo_existe(const Estoque *e, int codigo);
bool estoque_remover(Estoque *e, int indice);

#endif
