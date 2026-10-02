#ifndef UI_COMPONENTES_H
#define UI_COMPONENTES_H

#include <stdbool.h>
#include "raylib.h"
#include "ui/fonte.h"
#include "ui/tema.h"

Vector2 medir_texto(const char *txt, float tamanho, Peso peso);

void texto(const char *txt, float x, float y, float tamanho, Color cor);
void texto_forte(const char *txt, float x, float y, float tamanho, Color cor);
void texto_direita(const char *txt, float xDireita, float y, float tamanho, Color cor);

void card(Rectangle rec);
bool botao(Rectangle rec, const char *rotulo, EstiloBotao estilo);
bool botao_menu(Rectangle rec, const char *rotulo, bool ativo);
bool botao_remover(Rectangle rec, bool habilitado);
void desenhar_campo(Rectangle rec, const char *titulo, const char *conteudo, bool ativo);

#endif
