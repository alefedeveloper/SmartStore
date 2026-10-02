#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "app.h"

void desenhar_cabecalho(void);
void desenhar_menu(App *app);
void desenhar_rodape(const App *app);

/* Desenha o quadro inteiro: cabeçalho, menu, tela ativa e rodapé. */
void desenhar_app(App *app);

#endif
