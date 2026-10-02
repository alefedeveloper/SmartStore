/*

para fazer rodar o programa, abra o terminal e execute o seguinte comando:


set "PATH=C:\Users\alefe\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin;%PATH%" && cd /d C:\Users\alefe\OneDrive\Documentos\sp\supermecado && gcc main.c -o sistema.exe -IC:/Users/alefe/raylib/raylib-5.5_win64_mingw-w64/include -LC:/Users/alefe/raylib/raylib-5.5_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm && sistema.exe
 */

#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define LARGURA_JANELA     760
#define ALTURA_JANELA      520

#define MAX_PRODUTOS       100
#define TAM_NOME           35
#define TAM_CODIGO         10
#define TAM_PRECO          12
#define TAM_MENSAGEM       120

#define REPETICOES_TESTE   5000  
#define LINHAS_VISIVEIS    8      


#define AREA_X             175
#define AREA_Y             72
#define AREA_LARGURA       (LARGURA_JANELA - AREA_X)
#define AREA_ALTURA        388



typedef struct {
    int   codigo;
    char  nome[TAM_NOME];
    float preco;
} Produto;

typedef struct {
    Produto itens[MAX_PRODUTOS];
    int     quantidade;
} Estoque;

typedef enum { CRIT_PRECO, CRIT_CODIGO } Criterio;

typedef enum {
    ALG_BUBBLE,
    ALG_SELECTION,
    ALG_INSERTION,
    TOTAL_ALGORITMOS
} Algoritmo;

typedef enum { TELA_LISTA, TELA_CADASTRO, TELA_COMPARACAO } Tela;

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

typedef enum { ENTRADA_TEXTO, ENTRADA_INTEIRO, ENTRADA_DECIMAL } TipoEntrada;

typedef struct {
    Estoque   estoque;

    Tela      tela;
    ItemMenu  menuAtivo;
    Algoritmo algoritmo;
    int       rolagem;
    bool      sair;

    char      campos[TOTAL_CAMPOS][TAM_NOME];
    Campo     campoAtivo;


    char      mensagem[TAM_MENSAGEM];
    bool      mensagemErro;
} App;

typedef void (*FuncaoOrdenacao)(Produto[], int, Criterio);

static const char *NOMES_ALGORITMOS[TOTAL_ALGORITMOS] = {
    "Bubble Sort", "Selection Sort", "Insertion Sort"
};

static const char *ROTULOS_MENU[TOTAL_MENU] = {
    "+  Cadastrar",
    "Lista de produtos",
    "Ordenar por preco",
    "Ordenar por codigo",
    "Comparar algoritmos"
};

static const char *ROTULOS_CAMPOS[TOTAL_CAMPOS] = {
    "Codigo do produto", "Nome do produto", "Preco (ex: 12.50)"
};



static bool vem_antes(const Produto *a, const Produto *b, Criterio criterio)
{
    if (criterio == CRIT_PRECO)
        return a->preco < b->preco;
    return a->codigo < b->codigo;
}

static void trocar(Produto *a, Produto *b)
{
    Produto temp = *a;
    *a = *b;
    *b = temp;
}

static void bubble_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 0; i < n - 1; i++) {
        bool trocou = false;

        for (int j = 0; j < n - 1 - i; j++) {
            if (vem_antes(&v[j + 1], &v[j], criterio)) {
                trocar(&v[j], &v[j + 1]);
                trocou = true;
            }
        }
        if (!trocou) break;   
    }
}

static void selection_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 0; i < n - 1; i++) {
        int menor = i;

        for (int j = i + 1; j < n; j++) {
            if (vem_antes(&v[j], &v[menor], criterio))
                menor = j;
        }
        if (menor != i)
            trocar(&v[i], &v[menor]);
    }
}

static void insertion_sort(Produto v[], int n, Criterio criterio)
{
    for (int i = 1; i < n; i++) {
        Produto chave = v[i];
        int j = i - 1;

        while (j >= 0 && vem_antes(&chave, &v[j], criterio)) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}


static const FuncaoOrdenacao ORDENACOES[TOTAL_ALGORITMOS] = {
    bubble_sort, selection_sort, insertion_sort
};




static void estoque_adicionar(Estoque *e, int codigo, const char *nome, float preco)
{
    Produto *p = &e->itens[e->quantidade];

    p->codigo = codigo;
    strncpy(p->nome, nome, TAM_NOME - 1);
    p->nome[TAM_NOME - 1] = '\0';
    p->preco = preco;

    e->quantidade++;
}

static void estoque_carregar_iniciais(Estoque *e)
{
    e->quantidade = 0;
    estoque_adicionar(e, 142, "Arroz 5kg",     32.90f);
    estoque_adicionar(e, 108, "Feijao 1kg",     8.49f);
    estoque_adicionar(e, 175, "Acucar 1kg",     5.49f);
    estoque_adicionar(e, 121, "Cafe 500g",     17.90f);
    estoque_adicionar(e, 196, "Leite 1L",       5.89f);
    estoque_adicionar(e, 103, "Oleo 900ml",     7.99f);
    estoque_adicionar(e, 187, "Macarrao 500g",  4.99f);
    estoque_adicionar(e, 132, "Sal 1kg",        2.79f);
}

static bool estoque_codigo_existe(const Estoque *e, int codigo)
{
    for (int i = 0; i < e->quantidade; i++)
        if (e->itens[i].codigo == codigo)
            return true;
    return false;
}


static float converter_preco(const char *texto)
{
    char copia[TAM_PRECO + 1];

    strncpy(copia, texto, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    for (int i = 0; copia[i] != '\0'; i++)
        if (copia[i] == ',')
            copia[i] = '.';

    return (float)atof(copia);
}



static double medir_tempo(const Estoque *estoque, Algoritmo algoritmo)
{
    Produto base[MAX_PRODUTOS];
    Produto copia[MAX_PRODUTOS];
    int n = estoque->quantidade;

    if (n < 2) return 0.0;

    memcpy(base, estoque->itens, sizeof(Produto) * n);

    unsigned int semente = 12345u;                  
    for (int i = n - 1; i > 0; i--) {
        semente = semente * 1103515245u + 12345u;
        trocar(&base[i], &base[(semente >> 8) % (unsigned)(i + 1)]);
    }

    double inicio = GetTime();

    for (int r = 0; r < REPETICOES_TESTE; r++) {
        memcpy(copia, base, sizeof(Produto) * n);
        ORDENACOES[algoritmo](copia, n, CRIT_PRECO);
    }

    return ((GetTime() - inicio) * 1000.0) / REPETICOES_TESTE;
}




static const Color COR_FUNDO           = {245, 248, 253, 255};
static const Color COR_BRANCO          = {255, 255, 255, 255};
static const Color COR_AZUL_ESCURO     = { 18,  67, 126, 255};
static const Color COR_AZUL            = { 32, 105, 200, 255};
static const Color COR_AZUL_CLARO      = { 67, 145, 235, 255};
static const Color COR_AZUL_BEM_CLARO  = {231, 241, 254, 255};
static const Color COR_TEXTO           = { 35,  45,  62, 255};
static const Color COR_TEXTO_SUAVE     = {105, 118, 138, 255};
static const Color COR_BORDA           = {218, 226, 238, 255};
static const Color COR_VERDE           = { 35, 166, 107, 255};
static const Color COR_VERDE_CLARO     = {225, 247, 237, 255};
static const Color COR_VERMELHO        = {217,  65,  65, 255};
static const Color COR_LINHA_ALTERNADA = {248, 250, 253, 255};
static const Color COR_SOMBRA          = {210, 219, 231, 100};
static const Color COR_VEU             = {238, 244, 252, 245};

typedef struct {
    Color fundo, fundoHover, texto, textoHover;
} EstiloBotao;

static const EstiloBotao ESTILO_PRIMARIO = {
    {32, 105, 200, 255}, {25, 88, 170, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}
};
static const EstiloBotao ESTILO_SECUNDARIO = {
    {231, 241, 254, 255}, {67, 145, 235, 255}, {32, 105, 200, 255}, {255, 255, 255, 255}
};
static const EstiloBotao ESTILO_PERIGO = {
    {255, 235, 235, 255}, {217, 65, 65, 255}, {217, 65, 65, 255}, {255, 255, 255, 255}
};

static Font fonte;




static void texto(const char *txt, float x, float y, float tamanho, Color cor)
{
    DrawTextEx(fonte, txt, (Vector2){x, y}, tamanho, 1.0f, cor);
}

static void texto_direita(const char *txt, float xDireita, float y, float tamanho, Color cor)
{
    Vector2 medida = MeasureTextEx(fonte, txt, tamanho, 1.0f);
    texto(txt, xDireita - medida.x, y, tamanho, cor);
}

static void card(Rectangle rec)
{
    DrawRectangleRounded((Rectangle){rec.x + 3, rec.y + 4, rec.width, rec.height},
                         0.04f, 6, COR_SOMBRA);
    DrawRectangleRounded(rec, 0.04f, 6, COR_BRANCO);
    DrawRectangleRoundedLines(rec, 0.04f, 6, COR_BORDA);
}

static bool botao(Rectangle rec, const char *rotulo, EstiloBotao estilo)
{
    bool sobre = CheckCollisionPointRec(GetMousePosition(), rec);

    DrawRectangleRounded(rec, 0.18f, 8, sobre ? estilo.fundoHover : estilo.fundo);

    Vector2 medida = MeasureTextEx(fonte, rotulo, 15, 1);
    texto(rotulo,
          rec.x + rec.width  / 2 - medida.x / 2,
          rec.y + rec.height / 2 - medida.y / 2,
          15, sobre ? estilo.textoHover : estilo.texto);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool botao_menu(Rectangle rec, const char *rotulo, bool ativo)
{
    bool sobre = CheckCollisionPointRec(GetMousePosition(), rec);
    Color fundo, cor;

    if (ativo)      { fundo = COR_AZUL;           cor = COR_BRANCO; }
    else if (sobre) { fundo = COR_AZUL_BEM_CLARO; cor = COR_AZUL;   }
    else            { fundo = COR_BRANCO;         cor = COR_TEXTO;  }

    DrawRectangleRounded(rec, 0.14f, 6, fundo);
    texto(rotulo, rec.x + 15, rec.y + 11, 14, cor);

    return sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void desenhar_campo(Rectangle rec, const char *titulo, const char *conteudo, bool ativo)
{
    texto(titulo, rec.x, rec.y - 22, 14, COR_TEXTO);

    DrawRectangleRounded(rec, 0.08f, 5, COR_BRANCO);
    DrawRectangleRoundedLines(rec, 0.08f, 5, ativo ? COR_AZUL : COR_BORDA);
    texto(conteudo, rec.x + 10, rec.y + 10, 15, COR_TEXTO);

    /* cursor piscando */
    if (ativo && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 medida = MeasureTextEx(fonte, conteudo, 15, 1);
        DrawRectangle((int)(rec.x + 12 + medida.x), (int)rec.y + 9, 2, 18, COR_AZUL);
    }
}


static void processar_digitacao(char texto_campo[], int limite, TipoEntrada tipo)
{
    int tecla;

    while ((tecla = GetCharPressed()) > 0) {
        int tamanho = (int)strlen(texto_campo);
        bool permitido = false;

        if (tamanho >= limite - 1) continue;

        switch (tipo) {
            case ENTRADA_TEXTO:
                permitido = (tecla >= 32 && tecla <= 125);
                break;
            case ENTRADA_INTEIRO:
                permitido = (tecla >= '0' && tecla <= '9');
                break;
            case ENTRADA_DECIMAL:
                permitido = (tecla >= '0' && tecla <= '9') || tecla == '.' || tecla == ',';
                break;
        }

        if (permitido) {
            texto_campo[tamanho]     = (char)tecla;
            texto_campo[tamanho + 1] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        int tamanho = (int)strlen(texto_campo);
        if (tamanho > 0) texto_campo[tamanho - 1] = '\0';
    }
}

static void definir_mensagem(App *app, bool erro, const char *msg)
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

static void acao_abrir_cadastro(App *app)
{
    limpar_formulario(app);
    app->tela = TELA_CADASTRO;
    definir_mensagem(app, false, "Preencha os dados do novo produto.");
}

static void acao_listar(App *app)
{
    app->tela = TELA_LISTA;
    definir_mensagem(app, false, "Lista de produtos exibida.");
}

static void acao_ordenar(App *app, Criterio criterio)
{
    Estoque *e = &app->estoque;

    double inicio = GetTime();
    ORDENACOES[app->algoritmo](e->itens, e->quantidade, criterio);
    double ms = (GetTime() - inicio) * 1000.0;

    app->tela    = TELA_LISTA;
    app->rolagem = 0;

    definir_mensagem(app, false,
        TextFormat("Ordenado por %s com %s (%.4f ms).",
                   criterio == CRIT_PRECO ? "preco" : "codigo",
                   NOMES_ALGORITMOS[app->algoritmo], ms));
}

static void acao_comparar(App *app)
{
    for (int a = 0; a < TOTAL_ALGORITMOS; a++)
        app->tempos[a] = medir_tempo(&app->estoque, (Algoritmo)a);

    app->tela = TELA_COMPARACAO;
    definir_mensagem(app, false, "Comparacao realizada.");
}

static bool acao_salvar_produto(App *app)
{
    Estoque    *e      = &app->estoque;
    const char *nome   = app->campos[CAMPO_NOME];
    int         codigo = atoi(app->campos[CAMPO_CODIGO]);
    float       preco  = converter_preco(app->campos[CAMPO_PRECO]);

    if (e->quantidade >= MAX_PRODUTOS) {
        definir_mensagem(app, true, "Limite de produtos atingido.");
        return false;
    }
    if (codigo <= 0 || strlen(nome) == 0 || preco <= 0.0f) {
        definir_mensagem(app, true, "Preencha todos os campos corretamente.");
        return false;
    }
    if (estoque_codigo_existe(e, codigo)) {
        definir_mensagem(app, true, "Codigo ja cadastrado.");
        return false;
    }

    estoque_adicionar(e, codigo, nome, preco);
    definir_mensagem(app, false, "Produto cadastrado com sucesso!");

    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    app->rolagem   = e->quantidade > LINHAS_VISIVEIS ? e->quantidade - LINHAS_VISIVEIS : 0;
    return true;
}

static void acao_cancelar_cadastro(App *app)
{
    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    definir_mensagem(app, false, "Cadastro cancelado.");
}

static void executar_menu(App *app, ItemMenu item)
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




static void desenhar_cabecalho(void)
{
    DrawRectangle(0, 0, LARGURA_JANELA, 72, COR_BRANCO);
    DrawLine(0, 71, LARGURA_JANELA, 71, COR_BORDA);

    DrawCircle(37, 35, 19, COR_AZUL);
    DrawCircle(37, 35, 9, COR_AZUL_CLARO);

    texto("SmartStore", 70, 17, 22, COR_AZUL_ESCURO);
    texto("Gerenciamento de produtos", 70, 43, 12, COR_TEXTO_SUAVE);
}

static void desenhar_menu(App *app)
{
    DrawRectangle(0, AREA_Y, AREA_X, ALTURA_JANELA - AREA_Y, COR_BRANCO);
    DrawLine(AREA_X - 1, AREA_Y, AREA_X - 1, ALTURA_JANELA, COR_BORDA);

    texto("MENU", 20, 96, 11, COR_TEXTO_SUAVE);

    for (int i = 0; i < TOTAL_MENU; i++) {
        Rectangle rec = {15, (float)(125 + i * 47), 145, 38};

        if (botao_menu(rec, ROTULOS_MENU[i], app->menuAtivo == (ItemMenu)i))
            executar_menu(app, (ItemMenu)i);
    }

    
    texto("ALGORITMO", 20, 372, 11, COR_TEXTO_SUAVE);

    if (botao((Rectangle){15, 392, 145, 36}, NOMES_ALGORITMOS[app->algoritmo], ESTILO_SECUNDARIO)) {
        app->algoritmo = (Algoritmo)((app->algoritmo + 1) % TOTAL_ALGORITMOS);
        definir_mensagem(app, false,
            TextFormat("Algoritmo selecionado: %s.", NOMES_ALGORITMOS[app->algoritmo]));
    }

    if (botao((Rectangle){15, 445, 145, 38}, "Sair", ESTILO_PERIGO))
        app->sair = true;
}

static void desenhar_tela_lista(App *app)
{
    const Estoque *e = &app->estoque;
    Rectangle painel = {195, 105, 535, 330};

    if (app->tela == TELA_LISTA && CheckCollisionPointRec(GetMousePosition(), painel)) {
        int maxRolagem = e->quantidade > LINHAS_VISIVEIS ? e->quantidade - LINHAS_VISIVEIS : 0;

        app->rolagem -= (int)GetMouseWheelMove();
        if (app->rolagem < 0)          app->rolagem = 0;
        if (app->rolagem > maxRolagem) app->rolagem = maxRolagem;
    }

    card(painel);
    texto("Produtos cadastrados", 220, 125, 21, COR_AZUL_ESCURO);
    texto_direita(TextFormat("%d produtos", e->quantidade), 710, 129, 13, COR_TEXTO_SUAVE);

   
    DrawRectangleRounded((Rectangle){215, 165, 495, 32}, 0.08f, 5, COR_AZUL_BEM_CLARO);
    texto("CODIGO",  230, 174, 12, COR_AZUL_ESCURO);
    texto("PRODUTO", 325, 174, 12, COR_AZUL_ESCURO);
    texto("PRECO",   590, 174, 12, COR_AZUL_ESCURO);

   
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
    }

    if (e->quantidade > LINHAS_VISIVEIS)
        texto(TextFormat("Role o mouse para ver os outros %d produtos",
                         e->quantidade - LINHAS_VISIVEIS),
              220, 415, 12, COR_TEXTO_SUAVE);
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

static void desenhar_tela_cadastro(App *app)
{
    const Rectangle campos[TOTAL_CAMPOS] = {
        {330, 210, 280, 38},
        {330, 285, 280, 38},
        {330, 360, 280, 38}
    };

    atualizar_cadastro(app, campos);
    if (app->tela != TELA_CADASTRO) return;   

    DrawRectangle(AREA_X, AREA_Y, AREA_LARGURA, AREA_ALTURA, COR_VEU);
    card((Rectangle){240, 105, 450, 350});

    texto("Cadastrar produto", 280, 130, 23, COR_AZUL_ESCURO);
    texto("Adicione um novo produto ao sistema", 280, 160, 13, COR_TEXTO_SUAVE);

    for (int i = 0; i < TOTAL_CAMPOS; i++)
        desenhar_campo(campos[i], ROTULOS_CAMPOS[i], app->campos[i], app->campoAtivo == (Campo)i);

    if (botao((Rectangle){330, 415, 130, 38}, "Salvar produto", ESTILO_PRIMARIO))
        acao_salvar_produto(app);

    if (botao((Rectangle){475, 415, 130, 38}, "Cancelar", ESTILO_SECUNDARIO))
        acao_cancelar_cadastro(app);
}

static void desenhar_tela_comparacao(App *app)
{
    static const Color CORES_BARRAS[TOTAL_ALGORITMOS] = {
        {18, 67, 126, 255}, {32, 105, 200, 255}, {67, 145, 235, 255}
    };

    DrawRectangle(AREA_X, AREA_Y, AREA_LARGURA, AREA_ALTURA, COR_VEU);
    card((Rectangle){220, 100, 500, 345});

    texto("Comparacao de algoritmos", 255, 125, 22, COR_AZUL_ESCURO);
    texto("Tempo medio para ordenar os produtos por preco", 255, 155, 13, COR_TEXTO_SUAVE);

   
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
    texto(TextFormat("Mais rapido: %s", NOMES_ALGORITMOS[maisRapido]), 275, 404, 13, COR_VERDE);

    if (botao((Rectangle){590, 395, 110, 32}, "Fechar", ESTILO_PRIMARIO)) {
        app->tela      = TELA_LISTA;
        app->menuAtivo = MENU_LISTAR;
    }
}

static void desenhar_rodape(const App *app)
{
    DrawRectangle(AREA_X, 460, AREA_LARGURA, 60, COR_BRANCO);
    DrawLine(AREA_X, 459, LARGURA_JANELA, 459, COR_BORDA);

    DrawCircle(205, 490, 6, app->mensagemErro ? COR_VERMELHO : COR_VERDE);
    texto(app->mensagem, 220, 482, 13, app->mensagemErro ? COR_VERMELHO : COR_TEXTO_SUAVE);
}




static void iniciar_app(App *app)
{
    memset(app, 0, sizeof(App));

    estoque_carregar_iniciais(&app->estoque);

    app->tela      = TELA_LISTA;
    app->menuAtivo = MENU_LISTAR;
    app->algoritmo = ALG_BUBBLE;

    definir_mensagem(app, false, "Sistema pronto para uso.");
}

static void desenhar_app(App *app)
{
    ClearBackground(COR_FUNDO);

    desenhar_cabecalho();
    desenhar_menu(app);

    desenhar_tela_lista(app);                       

    if (app->tela == TELA_CADASTRO)   desenhar_tela_cadastro(app);
    if (app->tela == TELA_COMPARACAO) desenhar_tela_comparacao(app);

    desenhar_rodape(app);
}

int main(void)
{
    static App app;

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Sistema de Produtos");
    SetExitKey(KEY_NULL);        
    SetTargetFPS(60);

    fonte = GetFontDefault();
    iniciar_app(&app);

    while (!WindowShouldClose() && !app.sair) {
        BeginDrawing();
        desenhar_app(&app);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}