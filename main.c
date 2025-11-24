#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define CELL_SIZE 32
#define GRID_WIDTH  20
#define GRID_HEIGHT 15
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)
#define MAX_FRUITS 5

typedef struct {
    int x, y;
} Vector2i;

typedef struct {
    int x, y, w, h;
} Platform;


// FUNCIONES AUXILIARES

bool CellInsideGrid(Vector2i c) {
    return c.x >= 0 && c.x < GRID_WIDTH && c.y >= 0 && c.y < GRID_HEIGHT;
}

bool CollidesWithPlatform(Vector2i pos, Platform *plats, int count) {
    for (int i = 0; i < count; i++) {
        if (pos.x >= plats[i].x && pos.x < plats[i].x + plats[i].w &&
            pos.y >= plats[i].y && pos.y < plats[i].y + plats[i].h)
            return true;
    }
    return false;
}

bool CollidesWithSnake(Vector2i pos, Vector2i *snake, int len) {
    for (int i = 0; i < len; i++) {
        if (snake[i].x == pos.x && snake[i].y == pos.y)
            return true;
    }
    return false;
}

bool SnakeSupported(Vector2i *snake, int len, Platform *plats, int count) {
    // La serpiente está apoyada si al menos un segmento tiene algo debajo
    for (int i = 0; i < len; i++) {
        Vector2i below = {snake[i].x, snake[i].y + 1};
        if (below.y >= GRID_HEIGHT) return true;
        if (CollidesWithPlatform(below, plats, count)) return true;
    }
    return false;
}

// Genera una fruta en una posición libre del mapa
Vector2i GenerateFruit(Vector2i *snake, int len, Platform *plats, int platCount) {
    Vector2i pos;
    do {
        pos.x = rand() % GRID_WIDTH;
        pos.y = rand() % GRID_HEIGHT;
    } while (CollidesWithSnake(pos, snake, len) || CollidesWithPlatform(pos, plats, platCount));
    return pos;
}


int main(void) {
    InitWindow(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, "Snakebird by Pollos y Peibol");
    SetTargetFPS(60);
    srand(time(NULL));

    // --- Plataformas ---
    Platform platforms[3] = {
        {0, GRID_HEIGHT - 1, GRID_WIDTH, 1},    // suelo
        {5, 10, 5, 1},                         // plataforma media
        {12, 7, 3, 1}                          // plataforma alta
    };
    int platformCount = 3;

    // --- Serpiente ---
    Vector2i snake[MAX_SNAKE_LENGTH];
    int snakeLength = 4;
    snake[0] = (Vector2i){5, 5};
    snake[1] = (Vector2i){5, 6};
    snake[2] = (Vector2i){5, 7};
    snake[3] = (Vector2i){5, 8};

    // --- Frutas ---
    Vector2i fruits[MAX_FRUITS];
    int fruitCount = 3;
    for (int i = 0; i < fruitCount; i++)
        fruits[i] = GenerateFruit(snake, snakeLength, platforms, platformCount);

    // --- Variables ---
    bool moved = false;
    float gravityTimer = 0;
    const float gravityDelay = 0.25f; 

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        gravityTimer += dt;
        moved = false;

        // --- Movimiento manual ---
        Vector2i dir = {0, 0};
        if (IsKeyPressed(KEY_RIGHT)) { dir = (Vector2i){1, 0}; moved = true; }
        if (IsKeyPressed(KEY_LEFT))  { dir = (Vector2i){-1, 0}; moved = true; }
        if (IsKeyPressed(KEY_UP))    { dir = (Vector2i){0, -1}; moved = true; }
        if (IsKeyPressed(KEY_DOWN))  { dir = (Vector2i){0, 1}; moved = true; } // 🔹 AHORA FUNCIONA ↓

        if (moved) {
            Vector2i newHead = {snake[0].x + dir.x, snake[0].y + dir.y};

            // Validar colisiones
            if (CellInsideGrid(newHead) && 
                !CollidesWithPlatform(newHead, platforms, platformCount) &&
                !CollidesWithSnake(newHead, snake, snakeLength)) {
                
                // Mover cuerpo
                for (int i = snakeLength - 1; i > 0; i--)
                    snake[i] = snake[i - 1];
                snake[0] = newHead;
            }
        }

        // --- Comer frutas ---
        for (int i = 0; i < fruitCount; i++) {
            if (snake[0].x == fruits[i].x && snake[0].y == fruits[i].y) {
                // Comer fruta: crecer y regenerar
                if (snakeLength < MAX_SNAKE_LENGTH)
                    snake[snakeLength++] = snake[snakeLength - 1];
                fruits[i] = GenerateFruit(snake, snakeLength, platforms, platformCount);
            }
        }

        // --- Gravedad ---
        if (gravityTimer >= gravityDelay) {
            gravityTimer = 0;
            if (!SnakeSupported(snake, snakeLength, platforms, platformCount)) {
                bool canFall = true;
                for (int i = 0; i < snakeLength; i++) {
                    Vector2i below = {snake[i].x, snake[i].y + 1};
                    if (below.y >= GRID_HEIGHT || CollidesWithPlatform(below, platforms, platformCount)) {
                        canFall = false;
                        break;
                    }
                }
                if (canFall) {
                    for (int i = 0; i < snakeLength; i++)
                        snake[i].y += 1;
                }
            }
        }

        // --- DIBUJO ---
        BeginDrawing();
        ClearBackground((Color){25, 25, 40, 255});

        // Dibujar cuadrícula
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, (Color){25, 25, 40, 255});
            }
        }

        // Dibujar plataformas
        for (int i = 0; i < platformCount; i++) {
            DrawRectangle(platforms[i].x * CELL_SIZE, platforms[i].y * CELL_SIZE,
                          platforms[i].w * CELL_SIZE, platforms[i].h * CELL_SIZE, BROWN);
        }

        // Dibujar frutas
        for (int i = 0; i < fruitCount; i++) {
            DrawCircle(fruits[i].x * CELL_SIZE + CELL_SIZE / 2, 
                       fruits[i].y * CELL_SIZE + CELL_SIZE / 2, 
                       CELL_SIZE / 3, RED);
        }

        // Dibujar serpiente
        for (int i = 0; i < snakeLength; i++) {
            Color c = (i == 0) ? GREEN : LIME;
            DrawRectangle(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, c);
        }

        DrawText("Snakebird by Pollos y Peibol", 10, 10, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}