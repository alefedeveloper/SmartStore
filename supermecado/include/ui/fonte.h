#ifndef UI_FONTE_H
#define UI_FONTE_H

#include <stdbool.h>
#include "raylib.h"

typedef enum { PESO_NORMAL, PESO_FORTE, TOTAL_PESOS } Peso;

/* Localiza os arquivos .ttf em assets/fonts. Sem eles, usa a fonte padrão da raylib. */
bool fonte_carregar(void);

/* Fonte rasterizada no tamanho exato pedido (carregada sob demanda e guardada em cache). */
Font fonte_obter(Peso peso, float tamanho);

void fonte_descarregar(void);

#endif
