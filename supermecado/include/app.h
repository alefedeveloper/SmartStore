#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include "config.h"
#include "estoque.h"
#include "ordenacao.h"

typedef enum { TELA_LISTA, TELA_CADASTRO, TELA_COMPARACAO, TELA_REMOCAO } Tela;

typedef enum {
    MENU_CADASTRAR,
    MENU_LISTAR,
    MENU_ORDENAR_PRECO,
    MENU_ORDENAR_CODIGO,
    MENU_COMPARAR,
    TOTAL_MENU
} ItemMenu;

typedef enum {
    CAMPO_CODIGO,
    CAMPO_NOME,
    CAMPO_PRECO,
    TOTAL_CAMPOS
} Campo;

typedef struct {
    Estoque   estoque;

    Tela      tela;
    ItemMenu  menuAtivo;
    Algoritmo algoritmo;
    int       rolagem;
    bool      sair;

    char      campos[TOTAL_CAMPOS][TAM_NOME];
    Campo     campoAtivo;

    double    tempos[TOTAL_ALGORITMOS];

    int       indiceRemocao;

    char      arquivoDados[TAM_CAMINHO];
    bool      salvarHabilitado;

    char      mensagem[TAM_MENSAGEM];
    bool      mensagemErro;
} App;

void iniciar_app(App *app);
void definir_mensagem(App *app, bool erro, const char *msg);

void acao_abrir_cadastro(App *app);
void acao_listar(App *app);
void acao_ordenar(App *app, Criterio criterio);
void acao_comparar(App *app);
bool acao_salvar_produto(App *app);
void acao_cancelar_cadastro(App *app);
void acao_fechar_comparacao(App *app);
void acao_pedir_remocao(App *app, int indice);
void acao_confirmar_remocao(App *app);
void acao_cancelar_remocao(App *app);

void executar_menu(App *app, ItemMenu item);

#endif
