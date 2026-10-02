#ifndef ENTRADA_H
#define ENTRADA_H

typedef enum { ENTRADA_TEXTO, ENTRADA_INTEIRO, ENTRADA_DECIMAL } TipoEntrada;

void processar_digitacao(char texto_campo[], int limite, TipoEntrada tipo);

#endif
