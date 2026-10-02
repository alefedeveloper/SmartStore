#include "raylib.h"
#include "config.h"
#include "app.h"
#include "ui/layout.h"
#include "ui/fonte.h"

int main(void)
{
    static App app;

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Sistema de Produtos");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    fonte_carregar();
    iniciar_app(&app);

    while (!WindowShouldClose() && !app.sair) {
        BeginDrawing();
        desenhar_app(&app);
        EndDrawing();
    }

    fonte_descarregar();
    CloseWindow();
    return 0;
}
