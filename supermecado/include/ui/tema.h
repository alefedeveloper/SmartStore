#ifndef UI_TEMA_H
#define UI_TEMA_H

#include "raylib.h"

extern const Color COR_FUNDO;
extern const Color COR_BRANCO;
extern const Color COR_AZUL_ESCURO;
extern const Color COR_AZUL;
extern const Color COR_AZUL_CLARO;
extern const Color COR_AZUL_BEM_CLARO;
extern const Color COR_TEXTO;
extern const Color COR_TEXTO_SUAVE;
extern const Color COR_BORDA;
extern const Color COR_VERDE;
extern const Color COR_VERDE_CLARO;
extern const Color COR_VERMELHO;
extern const Color COR_LINHA_ALTERNADA;
extern const Color COR_SOMBRA;
extern const Color COR_VEU;

typedef struct {
    Color fundo, fundoHover, texto, textoHover;
} EstiloBotao;

extern const EstiloBotao ESTILO_PRIMARIO;
extern const EstiloBotao ESTILO_SECUNDARIO;
extern const EstiloBotao ESTILO_PERIGO;
extern const EstiloBotao ESTILO_PERIGO_FORTE;

#endif
