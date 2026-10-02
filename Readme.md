# SmartStore

Sistema de gerenciamento de produtos de supermercado com interface gráfica em
[raylib](https://www.raylib.com/), feito para a disciplina de Estrutura de Dados.
Permite cadastrar e listar produtos, ordená-los por preço ou código (Bubble,
Selection e Insertion Sort) e comparar o tempo de cada algoritmo.

## Persistência

Os produtos ficam salvos em `supermecado/data/produtos.txt`. O arquivo é lido ao
abrir o programa e regravado sempre que um produto é cadastrado ou removido (o
botão ✕ em cada linha da lista remove o produto, após confirmação). Se o arquivo
não existir, ele é criado com os produtos iniciais.

Formato: uma linha por produto, `codigo;preco;nome`. Linhas iniciadas por `#` são
comentários, e linhas inválidas são ignoradas (com aviso no rodapé). Preços são
arredondados para centavos e precisam estar entre R$ 0,01 e R$ 99.999,99:

```
# SmartStore - produtos cadastrados
# formato: codigo;preco;nome
142;32.90;Arroz 5kg
108;8.49;Feijão 1kg
```

## Estrutura

```
supermecado/
├── Makefile
├── assets/fonts/        # fonte Inter (licença SIL OFL, ver OFL.txt)
├── tests/                # testes da camada de dados (make test)
├── include/              # cabeçalhos (.h)
│   ├── config.h          # constantes de janela, layout e limites
│   ├── produto.h         # struct Produto e validação de preço
│   ├── estoque.h         # struct Estoque e operações
│   ├── persistencia.h    # leitura e gravação de data/produtos.txt
│   ├── ordenacao.h       # algoritmos de ordenação
│   ├── benchmark.h       # medição de tempo dos algoritmos
│   ├── entrada.h         # leitura do teclado e conversão de preço
│   ├── app.h             # estado da aplicação e ações do menu
│   └── ui/
│       ├── fonte.h       # carregamento da fonte TTF por peso e tamanho
│       ├── tema.h        # cores e estilos de botão
│       ├── componentes.h # texto, botões, cards e campos
│       ├── telas.h       # telas de lista, cadastro e comparação
│       └── layout.h      # cabeçalho, menu, rodapé e quadro completo
└── src/                  # implementações (.c), espelhando include/
    ├── main.c
    └── ui/
```

Os arquivos gerados vão para `build/` (objetos), `bin/` (executável) e `data/`
(produtos salvos). Ao distribuir o executável, mantenha a pasta `assets/` ao lado de `bin/`; sem ela,
o programa usa a fonte padrão da raylib (sem acentos).

## Compilação

Requisitos: `gcc`, `make` e `git`. **Não é preciso instalar a raylib**: se ela
não for encontrada no sistema, o Makefile baixa a raylib 5.5 para `vendor/` e a
compila automaticamente na primeira execução (leva cerca de 1 minuto).

```sh
cd supermecado
make            # compila
make run        # compila e executa
make debug      # compila com símbolos de depuração
make test       # roda os testes da camada de dados (não precisa de raylib)
make rebuild    # limpa e compila do zero
make clean      # remove build/ e bin/
make distclean  # remove também a raylib baixada (vendor/)
make info       # mostra a plataforma detectada e a raylib usada
```

A raylib é escolhida nesta ordem:

1. `RAYLIB_PATH`, se informado (`make RAYLIB_PATH=/caminho/da/raylib`);
2. raylib instalada no sistema, detectada via `pkg-config` (Linux/macOS);
3. raylib baixada em `vendor/` (use `make RAYLIB_VENDOR=1` para forçar).

### Linux

Para compilar a raylib a partir do código-fonte, são necessários os headers do
X11 e do OpenGL:

- Arch: `sudo pacman -S libx11 libxrandr libxinerama libxcursor libxi mesa`
- Debian/Ubuntu: `sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`
- Fedora: `sudo dnf install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel mesa-libGL-devel`

### macOS

Instale as ferramentas de linha de comando: `xcode-select --install`.

### Windows

Instale o [w64devkit](https://github.com/skeeto/w64devkit) ou o
[WinLibs](https://winlibs.com/) (ambos trazem `gcc` e `make`) e o
[Git](https://git-scm.com/). Depois, no terminal:

```bat
cd supermecado
mingw32-make run
```

(No w64devkit, o comando é simplesmente `make run`.)
