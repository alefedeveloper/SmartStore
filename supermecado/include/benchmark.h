#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "estoque.h"
#include "ordenacao.h"

/* Tempo médio (ms) para ordenar uma cópia embaralhada do estoque por preço. */
double medir_tempo(const Estoque *estoque, Algoritmo algoritmo);

#endif
