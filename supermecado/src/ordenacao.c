#include <stdbool.h>
#include "ordenacao.h"

const char *const NOMES_ALGORITMOS[TOTAL_ALGORITMOS] = {
    "Bubble Sort", "Selection Sort", "Insertion Sort"
};

const FuncaoOrdenacao ORDENACOES[TOTAL_ALGORITMOS] = {
    bubble_sort, selection_sort, insertion_sort
};

static bool vem_antes(const Produto *a, const Produto *b, Criterio criterio)
{
    if (criterio == CRIT_PRECO)
        return a->preco < b->preco;
    return a->codigo < b->codigo;
}

void trocar(Produto *a, Produto *b)
{
    Produto temp = *a;
    *a = *b;
    *b = temp;
}

void bubble_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 0; i < n - 1; i++) {
        bool trocou = false;

        for (int j = 0; j < n - 1 - i; j++) {
            if (vem_antes(&v[j + 1], &v[j], criterio)) {
                trocar(&v[j], &v[j + 1]);
                trocou = true;
            }
        }
        if (!trocou) break;
    }
}

void selection_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 0; i < n - 1; i++) {
        int menor = i;

        for (int j = i + 1; j < n; j++) {
            if (vem_antes(&v[j], &v[menor], criterio))
                menor = j;
        }
        if (menor != i)
            trocar(&v[i], &v[menor]);
    }
}

void insertion_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 1; i < n; i++) {
        Produto chave = v[i];
        int j = i - 1;

        while (j >= 0 && vem_antes(&chave, &v[j], criterio)) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}
