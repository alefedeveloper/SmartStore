#include "raylib.h"
#include "entrada.h"
#include "ui/componentes.h"
#include "ui/tema.h"
#include "ui/telas.h"

static const char *ROTULOS_CAMPOS[TOTAL_CAMPOS] = {
    "Código do produto", "Nome do produto", "Preço (ex: 12,50)"
};

void desenhar_tela_lista(App *app)
{
    const Estoque *e = &app->estoque;
    Rectangle painel = {195, 105, 535, 330};
    bool interativa = app->tela == TELA_LISTA;
    int remover = -1;

    if (interativa && CheckCollisionPointRec(GetMousePosition(), painel)) {
        int maxRolagem = e->quantidade > LINHAS_VISIVEIS ? e->quantidade - LINHAS_VISIVEIS : 0;

        app->rolagem -= (int)GetMouseWheelMove();
        if (app->rolagem < 0)          app->rolagem = 0;
        if (app->rolagem > maxRolagem) app->rolagem = maxRolagem;
    }

    card(painel);
    texto_forte("Produtos cadastrados", 220, 125, 21, COR_AZUL_ESCURO);
    texto_direita(TextFormat("%d produtos", e->quantidade), 710, 129, 13, COR_TEXTO_SUAVE);

    DrawRectangleRounded((Rectangle){215, 165, 495, 32}, 0.08f, 5, COR_AZUL_BEM_CLARO);
    texto_forte("CÓDIGO",  230, 174, 12, COR_AZUL_ESCURO);
    texto_forte("PRODUTO", 325, 174, 12, COR_AZUL_ESCURO);
    texto_forte("PREÇO",   590, 174, 12, COR_AZUL_ESCURO);

    for (int i = 0; i < LINHAS_VISIVEIS; i++) {
        int indice = app->rolagem + i;
        if (indice >= e->quantidade) break;

        const Produto *p = &e->itens[indice];
        int y = 203 + i * 27;

        if (i % 2 == 1)
            DrawRectangleRounded((Rectangle){215, (float)(y - 2), 495, 25}, 0.05f, 4, COR_LINHA_ALTERNADA);

        texto(TextFormat("%d", p->codigo),     230, (float)(y + 3), 13, COR_TEXTO);
        texto(p->nome,                         325, (float)(y + 3), 13, COR_TEXTO);
        texto(TextFormat("R$ %.2f", p->preco), 590, (float)(y + 3), 13, COR_AZUL);

        if (botao_remover((Rectangle){684, (float)(y + 1), 19, 19}, interativa))
            remover = indice;
    }

    if (e->quantidade == 0)
        texto("Nenhum produto cadastrado. Use \"+ Cadastrar\" no menu.", 230, 215, 13, COR_TEXTO_SUAVE);

    if (e->quantidade > LINHAS_VISIVEIS)
        texto(TextFormat("Role o mouse para ver os outros %d produtos",
                         e->quantidade - LINHAS_VISIVEIS),
              220, 415, 12, COR_TEXTO_SUAVE);

    /* a ação fica para o fim para não alterar o estoque no meio do desenho */
    if (remover >= 0)
        acao_pedir_remocao(app, remover);
}

static void atualizar_cadastro(App *app, const Rectangle campos[TOTAL_CAMPOS])
{
    static const TipoEntrada TIPOS[TOTAL_CAMPOS] = {ENTRADA_INTEIRO, ENTRADA_TEXTO, ENTRADA_DECIMAL};
    static const int LIMITES[TOTAL_CAMPOS]       = {TAM_CODIGO, TAM_NOME, TAM_PRECO};

    /* clique escolhe o campo */
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < TOTAL_CAMPOS; i++)
            if (CheckCollisionPointRec(GetMousePosition(), campos[i]))
                app->campoAtivo = (Campo)i;
    }

    if (IsKeyPressed(KEY_TAB))
        app->campoAtivo = (Campo)((app->campoAtivo + 1) % TOTAL_CAMPOS);

    if (IsKeyPressed(KEY_ESCAPE)) {
        acao_cancelar_cadastro(app);
        return;
    }

    processar_digitacao(app->campos[app->campoAtivo],
                        LIMITES[app->campoAtivo],
                        TIPOS[app->campoAtivo]);

    if (IsKeyPressed(KEY_ENTER)) {
        if (app->campoAtivo < TOTAL_CAMPOS - 1)
            app->campoAtivo = (Campo)(app->campoAtivo + 1);
        else
            acao_salvar_produto(app);
    }
}

void desenhar_tela_cadastro(App *app)
{
    const Rectangle campos[TOTAL_CAMPOS] = {
        {330, 205, 280, 38},
        {330, 275, 280, 38},
        {330, 345, 280, 38}
    };

    atualizar_cadastro(app, campos);
    if (app->tela != TELA_CADASTRO) return;

    DrawRectangle(AREA_X, AREA_Y, AREA_LARGURA, AREA_ALTURA, COR_VEU);
    card((Rectangle){240, 105, 450, 350});

    texto_forte("Cadastrar produto", 280, 124, 23, COR_AZUL_ESCURO);
    texto("Adicione um novo produto ao sistema", 280, 154, 13, COR_TEXTO_SUAVE);

    for (int i = 0; i < TOTAL_CAMPOS; i++)
        desenhar_campo(campos[i], ROTULOS_CAMPOS[i], app->campos[i], app->campoAtivo == (Campo)i);

    if (botao((Rectangle){330, 402, 130, 38}, "Salvar produto", ESTILO_PRIMARIO))
        acao_salvar_produto(app);

    if (botao((Rectangle){475, 402, 130, 38}, "Cancelar", ESTILO_SECUNDARIO))
        acao_cancelar_cadastro(app);
}

void desenhar_tela_comparacao(App *app)
{
    static const Color CORES_BARRAS[TOTAL_ALGORITMOS] = {
        {18, 67, 126, 255}, {32, 105, 200, 255}, {67, 145, 235, 255}
    };

    DrawRectangle(AREA_X, AREA_Y, AREA_LARGURA, AREA_ALTURA, COR_VEU);
    card((Rectangle){220, 100, 500, 345});

    texto_forte("Comparação de algoritmos", 255, 125, 22, COR_AZUL_ESCURO);
    texto("Tempo médio para ordenar os produtos por preço", 255, 155, 13, COR_TEXTO_SUAVE);

    double maior = 0.0;
    int maisRapido = 0;

    for (int a = 0; a < TOTAL_ALGORITMOS; a++) {
        if (app->tempos[a] > maior) maior = app->tempos[a];
        if (app->tempos[a] < app->tempos[maisRapido]) maisRapido = a;
    }
    if (maior <= 0.0) maior = 1.0;

    for (int a = 0; a < TOTAL_ALGORITMOS; a++) {
        float y = (float)(200 + a * 65);
        float larguraBarra = (float)(app->tempos[a] / maior) * 245.0f;
        if (larguraBarra < 4.0f) larguraBarra = 4.0f;

        texto(NOMES_ALGORITMOS[a], 260, y, 14, COR_TEXTO);
        DrawRectangleRounded((Rectangle){260, y + 25, 245, 17},          0.3f, 6, COR_AZUL_BEM_CLARO);
        DrawRectangleRounded((Rectangle){260, y + 25, larguraBarra, 17}, 0.3f, 6, CORES_BARRAS[a]);
        texto(TextFormat("%.6f ms", app->tempos[a]), 525, y + 24, 13, COR_TEXTO_SUAVE);
    }

    DrawRectangleRounded((Rectangle){260, 395, 260, 32}, 0.2f, 6, COR_VERDE_CLARO);
    texto_forte(TextFormat("Mais rápido: %s", NOMES_ALGORITMOS[maisRapido]), 275, 404, 13, COR_VERDE);

    if (botao((Rectangle){590, 395, 110, 32}, "Fechar", ESTILO_PRIMARIO))
        acao_fechar_comparacao(app);
}

void desenhar_tela_remocao(App *app)
{
    const Produto *p = &app->estoque.itens[app->indiceRemocao];

    if (IsKeyPressed(KEY_ESCAPE)) { acao_cancelar_remocao(app);  return; }
    if (IsKeyPressed(KEY_ENTER))  { acao_confirmar_remocao(app); return; }

    DrawRectangle(AREA_X, AREA_Y, AREA_LARGURA, AREA_ALTURA, COR_VEU);
    card((Rectangle){255, 160, 420, 210});

    texto_forte("Remover produto?", 285, 185, 21, COR_AZUL_ESCURO);

    texto_forte(p->nome, 285, 225, 15, COR_TEXTO);
    texto(TextFormat("Código %d  ·  R$ %.2f", p->codigo, p->preco), 285, 249, 13, COR_TEXTO_SUAVE);
    texto("O produto será apagado do arquivo produtos.txt.", 285, 280, 13, COR_TEXTO_SUAVE);

    if (botao((Rectangle){385, 312, 130, 38}, "Remover", ESTILO_PERIGO_FORTE))
        acao_confirmar_remocao(app);
    else if (botao((Rectangle){525, 312, 120, 38}, "Cancelar", ESTILO_SECUNDARIO))
        acao_cancelar_remocao(app);
}
