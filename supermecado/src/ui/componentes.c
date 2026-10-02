#include <math.h>
#include "ui/componentes.h"

#define ESPACAMENTO  0.4f

/* a Inter é visualmente menor que a fonte padrão no mesmo tamanho */
#define ESCALA_TEXTO 1.1f

static void desenhar_texto(Peso peso, const char *txt, float x, float y, float tamanho, Color cor)
{
    /* coordenadas inteiras evitam que o filtro borre os glifos */
    Vector2 pos = { floorf(x + 0.5f), floorf(y + 0.5f) };
    float t = floorf(tamanho * ESCALA_TEXTO + 0.5f);

    DrawTextEx(fonte_obter(peso, t), txt, pos, t, ESPACAMENTO, cor);
}

Vector2 medir_texto(const char *txt, float tamanho, Peso peso)
{
    float t = floorf(tamanho * ESCALA_TEXTO + 0.5f);

    return MeasureTextEx(fonte_obter(peso, t), txt, t, ESPACAMENTO);
}

void texto(const char *txt, float x, float y, float tamanho, Color cor)
{
    desenhar_texto(PESO_NORMAL, txt, x, y, tamanho, cor);
}

void texto_forte(const char *txt, float x, float y, float tamanho, Color cor)
{
    desenhar_texto(PESO_FORTE, txt, x, y, tamanho, cor);
}

void texto_direita(const char *txt, float xDireita, float y, float tamanho, Color cor)
{
    Vector2 medida = medir_texto(txt, tamanho, PESO_NORMAL);
    texto(txt, xDireita - medida.x, y, tamanho, cor);
}

void card(Rectangle rec)
{
    DrawRectangleRounded((Rectangle){rec.x + 3, rec.y + 4, rec.width, rec.height},
                         0.04f, 6, COR_SOMBRA);
    DrawRectangleRounded(rec, 0.04f, 6, COR_BRANCO);
    DrawRectangleRoundedLines(rec, 0.04f, 6, COR_BORDA);
}

bool botao(Rectangle rec, const char *rotulo, EstiloBotao estilo)
{
    bool sobre = CheckCollisionPointRec(GetMousePosition(), rec);

    DrawRectangleRounded(rec, 0.18f, 8, sobre ? estilo.fundoHover : estilo.fundo);

    Vector2 medida = medir_texto(rotulo, 14, PESO_FORTE);
    texto_forte(rotulo,
                rec.x + rec.width  / 2 - medida.x / 2,
                rec.y + rec.height / 2 - medida.y / 2,
                14, sobre ? estilo.textoHover : estilo.texto);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool botao_menu(Rectangle rec, const char *rotulo, bool ativo)
{
    bool sobre = CheckCollisionPointRec(GetMousePosition(), rec);
    Color fundo, cor;

    if (ativo)      { fundo = COR_AZUL;           cor = COR_BRANCO; }
    else if (sobre) { fundo = COR_AZUL_BEM_CLARO; cor = COR_AZUL;   }
    else            { fundo = COR_BRANCO;         cor = COR_TEXTO;  }

    DrawRectangleRounded(rec, 0.14f, 6, fundo);

    Vector2 medida = medir_texto(rotulo, 14, PESO_NORMAL);
    if (ativo) texto_forte(rotulo, rec.x + 15, rec.y + rec.height / 2 - medida.y / 2, 14, cor);
    else       texto(rotulo,       rec.x + 15, rec.y + rec.height / 2 - medida.y / 2, 14, cor);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

bool botao_remover(Rectangle rec, bool habilitado)
{
    bool  sobre  = habilitado && CheckCollisionPointRec(GetMousePosition(), rec);
    Color cor    = sobre ? COR_BRANCO : COR_TEXTO_SUAVE;
    float margem = 6.0f;

    if (sobre)
        DrawRectangleRounded(rec, 0.3f, 6, COR_VERMELHO);

    DrawLineEx((Vector2){rec.x + margem, rec.y + margem},
               (Vector2){rec.x + rec.width - margem, rec.y + rec.height - margem}, 1.8f, cor);
    DrawLineEx((Vector2){rec.x + rec.width - margem, rec.y + margem},
               (Vector2){rec.x + margem, rec.y + rec.height - margem}, 1.8f, cor);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void desenhar_campo(Rectangle rec, const char *titulo, const char *conteudo, bool ativo)
{
    texto_forte(titulo, rec.x, rec.y - 22, 13, COR_TEXTO);

    DrawRectangleRounded(rec, 0.08f, 5, COR_BRANCO);
    DrawRectangleRoundedLines(rec, 0.08f, 5, ativo ? COR_AZUL : COR_BORDA);

    Vector2 medida = medir_texto(conteudo, 15, PESO_NORMAL);
    float yTexto = rec.y + rec.height / 2 - medir_texto("Ag", 15, PESO_NORMAL).y / 2;
    texto(conteudo, rec.x + 10, yTexto, 15, COR_TEXTO);

    /* cursor piscando */
    if (ativo && ((int)(GetTime() * 2) % 2 == 0))
        DrawRectangle((int)(rec.x + 11 + medida.x), (int)rec.y + 10, 2, (int)rec.height - 20, COR_AZUL);
}
