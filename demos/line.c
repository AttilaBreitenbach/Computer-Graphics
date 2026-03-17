#include <SDL2/SDL.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_LINE_COUNT 100
#define PALETTE_SIZE 5

// ---- Struktúrák ----

typedef struct {
    Uint8 r, g, b;
} Color;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point p1;
    Point p2;
    Color color;
} Line;

// ---- Globális adatok ----

Line lines[MAX_LINE_COUNT];
int lineCount = 0;

Point tempPoint;
int hasFirstPoint = 0;

Color palette[PALETTE_SIZE] = {
    {255, 0, 0},     // piros
    {0, 255, 0},     // zöld
    {0, 0, 255},     // kék
    {255, 255, 0},   // sárga
    {255, 255, 255}  // fehér
};

Color currentColor = {255, 255, 255};

// ---- Paletta kattintás ellenőrzés ----

int checkPaletteClick(int x, int y) {
    for (int i = 0; i < PALETTE_SIZE; i++) {
        SDL_Rect rect = {10 + i * 50, 10, 40, 40};

        if (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h) {
            return i;
        }
    }
    return -1;
}

// ---- Főprogram ----

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL init hiba: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Szakasz rajzolo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                running = 0;
            }

            if (event.type == SDL_MOUSEMOTION) {
                printf("Mouse: %d %d\n", event.motion.x, event.motion.y);
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                int x = event.button.x;
                int y = event.button.y;

                // Paletta ellenőrzés
                int idx = checkPaletteClick(x, y);
                if (idx != -1) {
                    currentColor = palette[idx];
                    continue;
                }

                // Szakasz rajzolás
                if (!hasFirstPoint) {
                    tempPoint.x = x;
                    tempPoint.y = y;
                    hasFirstPoint = 1;
                } else {
                    if (lineCount < MAX_LINE_COUNT) {
                        lines[lineCount].p1 = tempPoint;
                        lines[lineCount].p2.x = x;
                        lines[lineCount].p2.y = y;
                        lines[lineCount].color = currentColor;
                        lineCount++;
                    }
                    hasFirstPoint = 0;
                }
            }
        }

        // ---- Rajzolás ----

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Szakaszok
        for (int i = 0; i < lineCount; i++) {
            SDL_SetRenderDrawColor(renderer,
                lines[i].color.r,
                lines[i].color.g,
                lines[i].color.b,
                255);

            SDL_RenderDrawLine(renderer,
                lines[i].p1.x, lines[i].p1.y,
                lines[i].p2.x, lines[i].p2.y);
        }

        // Paletta
        for (int i = 0; i < PALETTE_SIZE; i++) {
            SDL_Rect rect = {10 + i * 50, 10, 40, 40};

            SDL_SetRenderDrawColor(renderer,
                palette[i].r,
                palette[i].g,
                palette[i].b,
                255);

            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    // ---- Takarítás ----

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}