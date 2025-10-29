#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define CELLS_X 70
#define CELLS_Y 45
#define CELL_SIZE 15

int main(void)
{
    const int screenWidth = CELLS_X * CELL_SIZE;
    const int screenHeight = CELLS_Y * CELL_SIZE;

    InitWindow(screenWidth, screenHeight, "Juego de la Vida (Conway)");
    SetTargetFPS(10); // Velocidad de actualización

    int grid[CELLS_Y][CELLS_X] = {0};
    int next[CELLS_Y][CELLS_X] = {0};

    // Color personalizado para celdas vivas
    Color Homunculo = (Color){200, 255, 255, 255};

    srand(time(NULL));

    // Inicializa aleatoriamente la cuadrícula
    for (int y = 0; y < CELLS_Y; y++)
        for (int x = 0; x < CELLS_X; x++)
            grid[y][x] = rand() % 2;

    while (!WindowShouldClose())
    {
        // --- Lógica del juego ---
        for (int y = 0; y < CELLS_Y; y++)
        {
            for (int x = 0; x < CELLS_X; x++)
            {
                int vecinos = 0;

                for (int i = -1; i <= 1; i++)
                    for (int j = -1; j <= 1; j++)
                        if (!(i == 0 && j == 0))
                        {
                            int nx = x + j;
                            int ny = y + i;
                            if (nx >= 0 && nx < CELLS_X && ny >= 0 && ny < CELLS_Y)
                                vecinos += grid[ny][nx];
                        }

                if (grid[y][x] == 1 && (vecinos < 2 || vecinos > 3))
                    next[y][x] = 0;
                else if (grid[y][x] == 0 && vecinos == 3)
                    next[y][x] = 1;
                else
                    next[y][x] = grid[y][x];
            }
        }

        // Copiar el siguiente estado
        for (int y = 0; y < CELLS_Y; y++)
            for (int x = 0; x < CELLS_X; x++)
                grid[y][x] = next[y][x];

        // --- Dibujo ---
        BeginDrawing();
        ClearBackground(BLACK); // Fondo general negro

        // Dibujar las celdas vivas
        for (int y = 0; y < CELLS_Y; y++)
        {
            for (int x = 0; x < CELLS_X; x++)
            {
                if (grid[y][x])
                    DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, Homunculo);
            }
        }

        // --- Texto con fondo negro para mejor legibilidad ---
        const char *titulo = "Juego de la Vida (Conway)";
        const char *autor  = "Pablo Campuzano";
        int fontSize1 = 35;
        int fontSize2 = 25;

        // Dibujar rectángulos de fondo detrás del texto
        DrawRectangle(10, 10, MeasureText(titulo, fontSize1), fontSize1, BLACK);
        DrawRectangle(10, 45, MeasureText(autor, fontSize2), fontSize2, BLACK);

        // Dibujar texto encima
        DrawText(titulo, 10, 10, fontSize1, RAYWHITE);
        DrawText(autor, 10, 45, fontSize2, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
