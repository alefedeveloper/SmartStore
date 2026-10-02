#ifndef PRODUTO_H
#define PRODUTO_H

#include <stdbool.h>
#include "config.h"

/* Acima de ~131 mil, um float já não representa todos os centavos. */
#define PRECO_MINIMO  0.01f
#define PRECO_MAXIMO  99999.99f

typedef struct {
    int   codigo;
    char  nome[TAM_NOME];
    float preco;
} Produto;

/* Arredonda para centavos, que é a precisão gravada em produtos.txt. */
float produto_arredondar_preco(double valor);

/* Preço finito entre PRECO_MINIMO e PRECO_MAXIMO. Usada no cadastro e na leitura do arquivo. */
bool  produto_preco_valido(float preco);

/* Converte o texto digitado ("12,50" ou "12.50") em preço já arredondado. */
float produto_converter_preco(const char *texto);

#endif
