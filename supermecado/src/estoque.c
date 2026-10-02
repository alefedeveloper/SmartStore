#include <string.h>
#include "estoque.h"

void estoque_adicionar(Estoque *e, int codigo, const char *nome, float preco)
{
    Produto *p = &e->itens[e->quantidade];

    p->codigo = codigo;
    strncpy(p->nome, nome, TAM_NOME - 1);
    p->nome[TAM_NOME - 1] = '\0';
    p->preco = preco;

    e->quantidade++;
}

void estoque_carregar_iniciais(Estoque *e)
{
    e->quantidade = 0;
    estoque_adicionar(e, 142, "Arroz 5kg",     32.90f);
    estoque_adicionar(e, 108, "Feijão 1kg",     8.49f);
    estoque_adicionar(e, 175, "Açúcar 1kg",     5.49f);
    estoque_adicionar(e, 121, "Café 500g",     17.90f);
    estoque_adicionar(e, 196, "Leite 1L",       5.89f);
    estoque_adicionar(e, 103, "Óleo 900ml",     7.99f);
    estoque_adicionar(e, 187, "Macarrão 500g",  4.99f);
    estoque_adicionar(e, 132, "Sal 1kg",        2.79f);
}

bool estoque_codigo_existe(const Estoque *e, int codigo)
{
    for (int i = 0; i < e->quantidade; i++)
        if (e->itens[i].codigo == codigo)
            return true;
    return false;
}

bool estoque_remover(Estoque *e, int indice)
{
    if (indice < 0 || indice >= e->quantidade)
        return false;

    /* desloca os seguintes uma posição para trás, mantendo a ordem */
    for (int i = indice; i < e->quantidade - 1; i++)
        e->itens[i] = e->itens[i + 1];

    e->quantidade--;
    return true;
}
