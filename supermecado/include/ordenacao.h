#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "produto.h"

typedef enum { CRIT_PRECO, CRIT_CODIGO } Criterio;

typedef enum {
    ALG_BUBBLE,
    ALG_SELECTION,
    ALG_INSERTION,
    TOTAL_ALGORITMOS
} Algoritmo;

typedef void (*FuncaoOrdenacao)(Produto[], int, Criterio);

extern const char *const     NOMES_ALGORITMOS[TOTAL_ALGORITMOS];
extern const FuncaoOrdenacao ORDENACOES[TOTAL_ALGORITMOS];

void trocar(Produto *a, Produto *b);

void bubble_sort(Produto v[], int n, Criterio criterio);
void selection_sort(Produto v[], int n, Criterio criterio);
void insertion_sort(Produto v[], int n, Criterio criterio);

#endif
