#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "app.h"
#include "benchmark.h"
#include "persistencia.h"

#define NOME_ARQUIVO "produtos.txt"

static void resolver_caminho_dados(char destino[TAM_CAMINHO])
{
    /* data/ fica ao lado de bin/, independente do diretório de onde o programa foi aberto */
    char pasta[TAM_CAMINHO - sizeof("/" NOME_ARQUIVO)];
    snprintf(pasta, sizeof(pasta), "%s../data", GetApplicationDirectory());

    if (!DirectoryExists(pasta))
        MakeDirectory(pasta);

    snprintf(destino, TAM_CAMINHO, "%s/" NOME_ARQUIVO, pasta);
}

static void carregar_dados(App *app)
{
    int ignoradas = 0;

    resolver_caminho_dados(app->arquivoDados);
    app->salvarHabilitado = true;

    switch (persistencia_carregar(&app->estoque, app->arquivoDados, &ignoradas)) {
        case CARGA_OK:
            if (ignoradas > 0)
                definir_mensagem(app, true,
                    TextFormat("%d linha(s) inválida(s) ignorada(s) em " NOME_ARQUIVO ".", ignoradas));
            else
                definir_mensagem(app, false,
                    TextFormat("%d produtos carregados de " NOME_ARQUIVO ".", app->estoque.quantidade));
            break;

        case CARGA_INEXISTENTE:
            estoque_carregar_iniciais(&app->estoque);
            if (persistencia_salvar(&app->estoque, app->arquivoDados))
                definir_mensagem(app, false, NOME_ARQUIVO " criado com os produtos iniciais.");
            else
                definir_mensagem(app, true, "Não foi possível criar " NOME_ARQUIVO ".");
            break;

        case CARGA_ERRO:
            /* não sobrescreve um arquivo que existe mas não pôde ser lido */
            app->estoque.quantidade = 0;
            app->salvarHabilitado   = false;
            definir_mensagem(app, true, "Erro ao ler " NOME_ARQUIVO "; alterações não serão salvas.");
            break;
    }
}

static bool salvar_dados(App *app)
{
    return app->salvarHabilitado && persistencia_salvar(&app->estoque, app->arquivoDados);
}

void iniciar_app(App *app)
{
    memset(app, 0, sizeof(App));

    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    app->algoritmo = ALG_BUBBLE;

    carregar_dados(app);
}

void definir_mensagem(App *app, bool erro, const char *msg)
{
    strncpy(app->mensagem, msg, TAM_MENSAGEM - 1);
    app->mensagem[TAM_MENSAGEM - 1] = '\0';
    app->mensagemErro = erro;
}

static void limpar_formulario(App *app)
{
    for (int i = 0; i < TOTAL_CAMPOS; i++)
        app->campos[i][0] = '\0';
    app->campoAtivo = CAMPO_CODIGO;
}

void acao_abrir_cadastro(App *app)
{
    limpar_formulario(app);
    app->tela = TELA_CADASTRO;
    definir_mensagem(app, false, "Preencha os dados do novo produto.");
}

void acao_listar(App *app)
{
    app->tela = TELA_LISTA;
    definir_mensagem(app, false, "Lista de produtos exibida.");
}

void acao_ordenar(App *app, Criterio criterio)
{
    Estoque *e = &app->estoque;

    double inicio = GetTime();
    ORDENACOES[app->algoritmo](e->itens, e->quantidade, criterio);
    double ms = (GetTime() - inicio) * 1000.0;

    app->tela    = TELA_LISTA;
    app->rolagem = 0;

    definir_mensagem(app, false,
        TextFormat("Ordenado por %s com %s (%.4f ms).",
                   criterio == CRIT_PRECO ? "preço" : "código",
                   NOMES_ALGORITMOS[app->algoritmo], ms));
}

void acao_comparar(App *app)
{
    for (int a = 0; a < TOTAL_ALGORITMOS; a++)
        app->tempos[a] = medir_tempo(&app->estoque, (Algoritmo)a);

    app->tela = TELA_COMPARACAO;
    definir_mensagem(app, false, "Comparação realizada.");
}

bool acao_salvar_produto(App *app)
{
    Estoque    *e      = &app->estoque;
    const char *nome   = app->campos[CAMPO_NOME];
    int         codigo = atoi(app->campos[CAMPO_CODIGO]);
    float       preco  = produto_converter_preco(app->campos[CAMPO_PRECO]);

    if (e->quantidade >= MAX_PRODUTOS) {
        definir_mensagem(app, true, "Limite de produtos atingido.");
        return false;
    }
    if (codigo <= 0 || strlen(nome) == 0) {
        definir_mensagem(app, true, "Preencha todos os campos corretamente.");
        return false;
    }
    if (!produto_preco_valido(preco)) {
        definir_mensagem(app, true,
            TextFormat("O preço deve estar entre R$ %.2f e R$ %.2f.", PRECO_MINIMO, PRECO_MAXIMO));
        return false;
    }
    if (estoque_codigo_existe(e, codigo)) {
        definir_mensagem(app, true, "Código já cadastrado.");
        return false;
    }

    estoque_adicionar(e, codigo, nome, preco);

    if (salvar_dados(app))
        definir_mensagem(app, false, "Produto cadastrado com sucesso!");
    else
        definir_mensagem(app, true, "Produto cadastrado, mas não foi salvo em " NOME_ARQUIVO ".");

    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    app->rolagem   = e->quantidade > LINHAS_VISIVEIS ? e->quantidade - LINHAS_VISIVEIS : 0;
    return true;
}

void acao_cancelar_cadastro(App *app)
{
    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    definir_mensagem(app, false, "Cadastro cancelado.");
}

void acao_fechar_comparacao(App *app)
{
    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
}

void acao_pedir_remocao(App *app, int indice)
{
    if (indice < 0 || indice >= app->estoque.quantidade) return;

    app->indiceRemocao = indice;
    app->tela          = TELA_REMOCAO;
    definir_mensagem(app, false, "Confirme a remoção do produto.");
}

void acao_confirmar_remocao(App *app)
{
    Estoque *e = &app->estoque;
    char nome[TAM_NOME];

    app->tela = TELA_LISTA;

    if (app->indiceRemocao < 0 || app->indiceRemocao >= e->quantidade) return;

    strcpy(nome, e->itens[app->indiceRemocao].nome);
    estoque_remover(e, app->indiceRemocao);

    int maxRolagem = e->quantidade > LINHAS_VISIVEIS ? e->quantidade - LINHAS_VISIVEIS : 0;
    if (app->rolagem > maxRolagem) app->rolagem = maxRolagem;

    if (salvar_dados(app))
        definir_mensagem(app, false, TextFormat("Produto \"%s\" removido.", nome));
    else
        definir_mensagem(app, true, "Produto removido, mas não foi salvo em " NOME_ARQUIVO ".");
}

void acao_cancelar_remocao(App *app)
{
    app->tela = TELA_LISTA;
    definir_mensagem(app, false, "Remoção cancelada.");
}

void executar_menu(App *app, ItemMenu item)
{
    app->menuAtivo = item;

    switch (item) {
        case MENU_CADASTRAR:      acao_abrir_cadastro(app);       break;
        case MENU_LISTAR:         acao_listar(app);               break;
        case MENU_ORDENAR_PRECO:  acao_ordenar(app, CRIT_PRECO);  break;
        case MENU_ORDENAR_CODIGO: acao_ordenar(app, CRIT_CODIGO); break;
        case MENU_COMPARAR:       acao_comparar(app);             break;
        default: break;
    }
}
