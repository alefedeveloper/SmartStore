#include "raylib.h"
#include "ui/componentes.h"
#include "ui/tema.h"
#include "ui/telas.h"
#include "ui/layout.h"

static const char *ROTULOS_MENU[TOTAL_MENU] = {
    "+  Cadastrar",
    "Lista de produtos",
    "Ordenar por preço",
    "Ordenar por código",
    "Comparar algoritmos"
};

void desenhar_cabecalho(void)
{
    DrawRectangle(0, 0, LARGURA_JANELA, 72, COR_BRANCO);
    DrawLine(0, 71, LARGURA_JANELA, 71, COR_BORDA);

    DrawCircle(37, 35, 19, COR_AZUL);
    DrawCircle(37, 35, 9, COR_AZUL_CLARO);

    texto_forte("SmartStore", 70, 17, 22, COR_AZUL_ESCURO);
    texto("Gerenciamento de produtos", 70, 43, 12, COR_TEXTO_SUAVE);
}

void desenhar_menu(App *app)
{
    DrawRectangle(0, AREA_Y, AREA_X, ALTURA_JANELA - AREA_Y, COR_BRANCO);
    DrawLine(AREA_X - 1, AREA_Y, AREA_X - 1, ALTURA_JANELA, COR_BORDA);

    texto_forte("MENU", 20, 96, 11, COR_TEXTO_SUAVE);

    for (int i = 0; i < TOTAL_MENU; i++) {
        Rectangle rec = {15, (float)(125 + i * 47), 145, 38};

        if (botao_menu(rec, ROTULOS_MENU[i], app->menuAtivo == (ItemMenu)i))
            executar_menu(app, (ItemMenu)i);
    }

    texto_forte("ALGORITMO", 20, 372, 11, COR_TEXTO_SUAVE);

    if (botao((Rectangle){15, 392, 145, 36}, NOMES_ALGORITMOS[app->algoritmo], ESTILO_SECUNDARIO)) {
        app->algoritmo = (Algoritmo)((app->algoritmo + 1) % TOTAL_ALGORITMOS);
        definir_mensagem(app, false,
            TextFormat("Algoritmo selecionado: %s.", NOMES_ALGORITMOS[app->algoritmo]));
    }

    if (botao((Rectangle){15, 445, 145, 38}, "Sair", ESTILO_PERIGO))
        app->sair = true;
}

void desenhar_rodape(const App *app)
{
    DrawRectangle(AREA_X, 460, AREA_LARGURA, 60, COR_BRANCO);
    DrawLine(AREA_X, 459, LARGURA_JANELA, 459, COR_BORDA);

    DrawCircle(205, 490, 6, app->mensagemErro ? COR_VERMELHO : COR_VERDE);
    texto(app->mensagem, 220, 482, 13, app->mensagemErro ? COR_VERMELHO : COR_TEXTO_SUAVE);
}

void desenhar_app(App *app)
{
    ClearBackground(COR_FUNDO);

    desenhar_cabecalho();
    desenhar_menu(app);

    desenhar_tela_lista(app);

    if (app->tela == TELA_CADASTRO)   desenhar_tela_cadastro(app);
    if (app->tela == TELA_COMPARACAO) desenhar_tela_comparacao(app);
    if (app->tela == TELA_REMOCAO)    desenhar_tela_remocao(app);

    desenhar_rodape(app);
}
