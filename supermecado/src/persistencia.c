#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "persistencia.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define TAM_LINHA  256

/*
 * Troca 'destino' por 'origem' de forma atômica: ou o destino continua o
 * antigo, ou já é o novo; nunca fica sem arquivo. Se falhar, nada muda.
 * No Windows, rename() falha quando o destino existe, por isso MoveFileEx.
 * Os caminhos vêm em ANSI (é o que a raylib devolve), daí a versão "A".
 */
static bool substituir_arquivo(const char *origem, const char *destino)
{
#ifdef _WIN32
    return MoveFileExA(origem, destino, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(origem, destino) == 0;
#endif
}

static void remover_quebra(char *linha)
{
    size_t n = strlen(linha);

    while (n > 0 && (linha[n - 1] == '\n' || linha[n - 1] == '\r'))
        linha[--n] = '\0';
}

static bool interpretar_linha(char *linha, int *codigo, float *preco, char **nome)
{
    char *fim;

    long c = strtol(linha, &fim, 10);
    if (fim == linha || *fim != ';' || c <= 0 || c > 999999999L) return false;

    char *inicioPreco = fim + 1;
    double p = strtod(inicioPreco, &fim);
    if (fim == inicioPreco || *fim != ';') return false;

    float arredondado = produto_arredondar_preco(p);
    if (!produto_preco_valido(arredondado)) return false;

    *nome = fim + 1;
    if (**nome == '\0') return false;

    *codigo = (int)c;
    *preco  = arredondado;
    return true;
}

ResultadoCarga persistencia_carregar(Estoque *e, const char *caminho, int *ignoradas)
{
    FILE *arquivo = fopen(caminho, "r");
    char linha[TAM_LINHA];

    *ignoradas = 0;

    if (arquivo == NULL)
        return errno == ENOENT ? CARGA_INEXISTENTE : CARGA_ERRO;

    e->quantidade = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        int    codigo;
        float  preco;
        char  *nome;

        /* linha maior que o buffer: descarta o restante e a considera inválida */
        if (strchr(linha, '\n') == NULL && !feof(arquivo)) {
            int ch;
            while ((ch = fgetc(arquivo)) != '\n' && ch != EOF) {}
            (*ignoradas)++;
            continue;
        }

        remover_quebra(linha);
        if (linha[0] == '\0' || linha[0] == '#') continue;

        if (e->quantidade >= MAX_PRODUTOS
            || !interpretar_linha(linha, &codigo, &preco, &nome)
            || estoque_codigo_existe(e, codigo)) {
            (*ignoradas)++;
            continue;
        }

        estoque_adicionar(e, codigo, nome, preco);
    }

    bool falhou = ferror(arquivo);
    fclose(arquivo);
    return falhou ? CARGA_ERRO : CARGA_OK;
}

bool persistencia_salvar(const Estoque *e, const char *caminho)
{
    char temporario[TAM_CAMINHO + 8];
    snprintf(temporario, sizeof(temporario), "%s.tmp", caminho);

    /* grava num arquivo temporário e só depois substitui o original,
       para não corromper os dados se a gravação falhar no meio */
    FILE *arquivo = fopen(temporario, "w");
    if (arquivo == NULL) return false;

    fprintf(arquivo, "# SmartStore - produtos cadastrados\n");
    fprintf(arquivo, "# formato: codigo;preco;nome\n");

    for (int i = 0; i < e->quantidade; i++) {
        const Produto *p = &e->itens[i];
        fprintf(arquivo, "%d;%.2f;%s\n", p->codigo, p->preco, p->nome);
    }

    bool ok = !ferror(arquivo);
    if (fclose(arquivo) != 0) ok = false;

    if (!ok) {
        remove(temporario);
        return false;
    }

    if (!substituir_arquivo(temporario, caminho)) {
        remove(temporario);   /* o original ficou intacto */
        return false;
    }
    return true;
}
