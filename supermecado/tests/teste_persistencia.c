/*
 * Testes da camada de dados (estoque + persistência), sem janela nem raylib.
 * Uso: teste_persistencia <pasta-temporaria>      (via: make test)
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "estoque.h"
#include "persistencia.h"
#include "produto.h"

static const char *pasta;
static int falhas, verificacoes;

#define VERIFICA(cond) do {                                              \
        verificacoes++;                                                  \
        if (!(cond)) {                                                   \
            printf("  FALHOU %s:%d: %s\n", __func__, __LINE__, #cond);  \
            falhas++;                                                    \
        }                                                                \
    } while (0)

#define PRECO_IGUAL(a, b) (fabsf((a) - (b)) < 0.0001f)

static const char *caminho(const char *arquivo)
{
    static char buffer[TAM_CAMINHO];
    snprintf(buffer, sizeof(buffer), "%s/%s", pasta, arquivo);
    return buffer;
}

static void escrever(const char *arquivo, const char *conteudo)
{
    FILE *f = fopen(caminho(arquivo), "wb");
    fputs(conteudo, f);
    fclose(f);
}

/* ---------------------------------------------------------------- */

static void teste_arquivo_inexistente(void)
{
    Estoque e;
    int ignoradas;

    remove(caminho("nao_existe.txt"));
    VERIFICA(persistencia_carregar(&e, caminho("nao_existe.txt"), &ignoradas) == CARGA_INEXISTENTE);
}

static void teste_ida_e_volta(void)
{
    Estoque e = {0}, lido;
    int ignoradas;

    estoque_adicionar(&e, 1, "Feijão; tipo 1", 8.49f);
    estoque_adicionar(&e, 2, "Será removido", 1.00f);
    estoque_adicionar(&e, 3, "Açúcar", 5.49f);
    VERIFICA(estoque_remover(&e, 1));
    VERIFICA(!estoque_remover(&e, 5));

    VERIFICA(persistencia_salvar(&e, caminho("ida_volta.txt")));
    VERIFICA(persistencia_carregar(&lido, caminho("ida_volta.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(ignoradas == 0);
    VERIFICA(lido.quantidade == 2);
    VERIFICA(lido.itens[0].codigo == 1 && strcmp(lido.itens[0].nome, "Feijão; tipo 1") == 0);
    VERIFICA(lido.itens[1].codigo == 3 && PRECO_IGUAL(lido.itens[1].preco, 5.49f));
}

static void teste_sobrescreve_arquivo_existente(void)
{
    Estoque e = {0}, lido;
    int ignoradas;

    estoque_adicionar(&e, 1, "A", 1.00f);
    estoque_adicionar(&e, 2, "B", 2.00f);
    VERIFICA(persistencia_salvar(&e, caminho("sobrescreve.txt")));

    estoque_remover(&e, 0);
    VERIFICA(persistencia_salvar(&e, caminho("sobrescreve.txt")));

    VERIFICA(persistencia_carregar(&lido, caminho("sobrescreve.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(lido.quantidade == 1 && lido.itens[0].codigo == 2);

    FILE *tmp = fopen(caminho("sobrescreve.txt.tmp"), "r");
    VERIFICA(tmp == NULL);   /* não deixa o temporário para trás */
    if (tmp) fclose(tmp);
}

static void teste_arquivo_editado_a_mao(void)
{
    Estoque lido;
    int ignoradas;
    char longa[400];

    memset(longa, 'a', sizeof(longa) - 1);
    longa[sizeof(longa) - 1] = '\0';

    char conteudo[1024];
    snprintf(conteudo, sizeof(conteudo),
        "# comentario\r\n"
        "\r\n"
        "10;2.50;Ok CRLF\r\n"
        "abc;1;codigo invalido\n"
        "11;0;preco zero\n"
        "12;3.0;\n"                          /* sem nome */
        "13;x;preco invalido\n"
        "10;9.99;codigo duplicado\n"
        "-5;1;codigo negativo\n"
        "14;1.5;%s\n"                        /* linha maior que o buffer */
        "15;1.00;Sem quebra no fim", longa);
    escrever("editado.txt", conteudo);

    VERIFICA(persistencia_carregar(&lido, caminho("editado.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(lido.quantidade == 2);
    VERIFICA(ignoradas == 7);
    VERIFICA(strcmp(lido.itens[0].nome, "Ok CRLF") == 0);
    VERIFICA(strcmp(lido.itens[1].nome, "Sem quebra no fim") == 0);
}

static void teste_limite_de_produtos(void)
{
    Estoque lido;
    int ignoradas;
    FILE *f = fopen(caminho("limite.txt"), "w");

    for (int i = 1; i <= MAX_PRODUTOS + 5; i++)
        fprintf(f, "%d;1.00;P%d\n", i, i);
    fclose(f);

    VERIFICA(persistencia_carregar(&lido, caminho("limite.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(lido.quantidade == MAX_PRODUTOS);
    VERIFICA(ignoradas == 5);
}

static void teste_salvar_em_pasta_inexistente_falha(void)
{
    Estoque e = {0};
    estoque_adicionar(&e, 1, "A", 1.00f);
    VERIFICA(!persistencia_salvar(&e, caminho("pasta_que_nao_existe/x.txt")));
}

static void teste_preco_valido(void)
{
    VERIFICA(produto_preco_valido(0.01f));
    VERIFICA(produto_preco_valido(12.50f));
    VERIFICA(produto_preco_valido(PRECO_MAXIMO));

    VERIFICA(!produto_preco_valido(0.0f));
    VERIFICA(!produto_preco_valido(-1.0f));
    VERIFICA(!produto_preco_valido(PRECO_MAXIMO + 1.0f));
    VERIFICA(!produto_preco_valido(NAN));
    VERIFICA(!produto_preco_valido(INFINITY));
}

static void teste_converter_preco(void)
{
    VERIFICA(PRECO_IGUAL(produto_converter_preco("12,50"), 12.50f));
    VERIFICA(PRECO_IGUAL(produto_converter_preco("12.50"), 12.50f));
    VERIFICA(PRECO_IGUAL(produto_converter_preco("1,999"), 2.00f));
    VERIFICA(PRECO_IGUAL(produto_converter_preco("7"),     7.00f));
    VERIFICA(produto_converter_preco("") == 0.0f);
}

/* ---- regressões da revisão ---------------------------------------- */

static void teste_preco_digitado_abaixo_de_um_centavo_e_rejeitado(void)
{
    /* "0,001" passava no cadastro (> 0), era gravado como 0.00 e sumia ao recarregar */
    VERIFICA(!produto_preco_valido(produto_converter_preco("0,001")));
    VERIFICA(!produto_preco_valido(produto_converter_preco("0,004")));
}

static void teste_preco_valido_sobrevive_a_ida_e_volta(void)
{
    /* invariante: todo preço que o cadastro aceita volta igual do arquivo */
    const char *digitados[] = { "0,01", "0,1", "1,999", "32,90", "99999,99", "50000" };
    Estoque e = {0}, lido;
    int ignoradas;

    for (int i = 0; i < 6; i++) {
        float p = produto_converter_preco(digitados[i]);
        VERIFICA(produto_preco_valido(p));
        estoque_adicionar(&e, i + 1, digitados[i], p);
    }

    VERIFICA(persistencia_salvar(&e, caminho("precos.txt")));
    VERIFICA(persistencia_carregar(&lido, caminho("precos.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(ignoradas == 0 && lido.quantidade == 6);

    for (int i = 0; i < lido.quantidade; i++)
        VERIFICA(PRECO_IGUAL(lido.itens[i].preco, e.itens[i].preco));
}


static void teste_rejeita_preco_nao_finito_ou_absurdo(void)
{
    Estoque lido;
    int ignoradas;

    escrever("nao_finito.txt",
        "1;nan;NaN\n"
        "2;inf;Infinito\n"
        "3;-inf;Menos infinito\n"
        "4;1e30;Absurdo\n"
        "5;3.00;Valido\n");

    VERIFICA(persistencia_carregar(&lido, caminho("nao_finito.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(lido.quantidade == 1 && lido.itens[0].codigo == 5);
    VERIFICA(ignoradas == 4);
}

static void teste_arredonda_preco_para_centavos_ao_carregar(void)
{
    Estoque lido;
    int ignoradas;

    escrever("centavos.txt",
        "1;1.999;Arredonda para cima\n"
        "2;0.004;Abaixo de um centavo\n");

    VERIFICA(persistencia_carregar(&lido, caminho("centavos.txt"), &ignoradas) == CARGA_OK);
    VERIFICA(lido.quantidade == 1);
    VERIFICA(lido.itens[0].codigo == 1 && PRECO_IGUAL(lido.itens[0].preco, 2.00f));
    VERIFICA(ignoradas == 1);
}

/* ---------------------------------------------------------------- */

int main(int argc, char **argv)
{
    pasta = argc > 1 ? argv[1] : ".";

    teste_arquivo_inexistente();
    teste_ida_e_volta();
    teste_sobrescreve_arquivo_existente();
    teste_arquivo_editado_a_mao();
    teste_limite_de_produtos();
    teste_salvar_em_pasta_inexistente_falha();
    teste_preco_valido();
    teste_converter_preco();
    teste_preco_digitado_abaixo_de_um_centavo_e_rejeitado();
    teste_preco_valido_sobrevive_a_ida_e_volta();
    teste_rejeita_preco_nao_finito_ou_absurdo();
    teste_arredonda_preco_para_centavos_ao_carregar();

    if (falhas == 0)
        printf("OK: %d verificações passaram\n", verificacoes);
    else
        printf("%d de %d verificações FALHARAM\n", falhas, verificacoes);

    return falhas == 0 ? 0 : 1;
}
