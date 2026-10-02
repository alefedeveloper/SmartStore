#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <stdbool.h>
#include "estoque.h"

/*
 * Arquivo texto, um produto por linha:  codigo;preco;nome
 * O nome fica por último para poder conter ';'. Linhas iniciadas
 * por '#' e linhas vazias são ignoradas.
 */

typedef enum {
    CARGA_OK,
    CARGA_INEXISTENTE,
    CARGA_ERRO
} ResultadoCarga;

/* Carrega o estoque do arquivo. Em 'ignoradas' devolve quantas linhas inválidas foram puladas. */
ResultadoCarga persistencia_carregar(Estoque *e, const char *caminho, int *ignoradas);

/* Regrava o arquivo inteiro com o estoque atual. */
bool persistencia_salvar(const Estoque *e, const char *caminho);

#endif
