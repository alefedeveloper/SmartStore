#include <string.h>
#include "raylib.h"
#include "benchmark.h"

double medir_tempo(const Estoque *estoque, Algoritmo algoritmo)
{
    Produto base[MAX_PRODUTOS];
    Produto copia[MAX_PRODUTOS];
    int n = estoque->quantidade;

    if (n < 2) return 0.0;

    memcpy(base, estoque->itens, sizeof(Produto) * n);

    unsigned int semente = 12345u;
    for (int i = n - 1; i > 0; i--) {
        semente = semente * 1103515245u + 12345u;
        trocar(&base[i], &base[(semente >> 8) % (unsigned)(i + 1)]);
    }

    double inicio = GetTime();

    for (int r = 0; r < REPETICOES_TESTE; r++) {
        memcpy(copia, base, sizeof(Produto) * n);
        ORDENACOES[algoritmo](copia, n, CRIT_PRECO);
    }

    return ((GetTime() - inicio) * 1000.0) / REPETICOES_TESTE;
}
