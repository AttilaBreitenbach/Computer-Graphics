#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 700

#define MAX_CIRCLE_COUNT 50
#define PALETTE_SIZE 5

// ---- Struktúrák ----

typedef struct {
    Uint8 r, g, b;
} Color;

typedef struct {
    int x, y;
    int radius;
    Color color;
} Circle;

// ---- Globális változók ----

Circle circles[MAX_CIRCLE_COUNT];
int circleCount = 0;

Color palette[PALETTE_SIZE] = {
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {255,255,0},
    {255,255,255}
};

Color currentColor = {255,255,255};

// mód: 0 = lépésszám, 1 = szög, 2 = hossz
int drawMode = 0;

// mozgatás
int draggingIndex = -1;
int offsetX, offsetY;

// ---- Kör rajzolás ----

// 1. lépésszám
void drawCircle_steps(SDL_Renderer* r, Circle c, int steps) {
    double step = 2 * M_PI / steps;

    for (int i = 0; i < steps; i++) {
        double t1 = i * step;
        double t2 = (i + 1) * step;

        int x1 = c.x + c.radius * cos(t1);
        int y1 = c.y + c.radius * sin(t1);

        int x2 = c.x + c.radius * cos(t2);
        int y2 = c.y + c.radius * sin(t2);

        SDL_RenderDrawLine(r, x1, y1, x2, y2);
    }
}

// 2. szög
void drawCircle_angle(SDL_Renderer* r, Circle c, double angleStep) {
    for (double t = 0; t < 2*M_PI; t += angleStep) {

        double t2 = t + angleStep;

        int x1 = c.x + c.radius * cos(t);
        int y1 = c.y + c.radius * sin(t);

        int x2 = c.x + c.radius * cos(t2);
        int y2 = c.y + c.radius * sin(t2);

        SDL_RenderDrawLine(r, x1, y1, x2, y2);
    }
}

// 3. max hossz
void drawCircle_length(SDL_Renderer* r, Circle c, double maxLen) {
    double kerulet = 2 * M_PI * c.radius;
    int steps = (int)(kerulet / maxLen);

    if (steps < 12) steps = 12;

    drawCircle_steps(r, c, steps);
}

// ---- Hover ellenőrzés ----

int isMouseOverCircle(int mx, int my, Circle c) {
    int dx = mx - c.x;
    int dy = my - c.y;
    return dx*dx + dy*dy <= c.radius*c.radius;
}

// ---- Jel rajzolás ----

void drawPlus(SDL_Renderer* r, Circle c) {
    int s = c.radius / 2;

    SDL_RenderDrawLine(r, c.x - s, c.y, c.x + s, c.y);
    SDL_RenderDrawLine(r, c.x, c.y - s, c.x, c.y + s);
}

// ---- Paletta ----

int checkPaletteClick(int x, int y) {
    for (int i = 0; i < PALETTE_SIZE; i++) {
        SDL_Rect rect = {10 + i*50, 10, 40, 40};

        if (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h) {
            return i;
        }
    }
    return -1;
}

// ---- Főprogram ----

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Kor kezelo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    int mx = 0, my = 0;

    while (running) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_MOUSEMOTION) {
                mx = event.motion.x;
                my = event.motion.y;

                printf("Mouse: %d %d\n", mx, my);

                if (draggingIndex != -1) {
                    circles[draggingIndex].x = mx - offsetX;
                    circles[draggingIndex].y = my - offsetY;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                int x = event.button.x;
                int y = event.button.y;

                // paletta
                int idx = checkPaletteClick(x, y);
                if (idx != -1) {
                    currentColor = palette[idx];
                    continue;
                }

                // meglévő kör -> mozgatás
                for (int i = 0; i < circleCount; i++) {
                    if (isMouseOverCircle(x, y, circles[i])) {
                        draggingIndex = i;
                        offsetX = x - circles[i].x;
                        offsetY = y - circles[i].y;
                        break;
                    }
                }

                // új kör
                if (draggingIndex == -1 && circleCount < MAX_CIRCLE_COUNT) {
                    circles[circleCount].x = x;
                    circles[circleCount].y = y;
                    circles[circleCount].radius = 40;
                    circles[circleCount].color = currentColor;
                    circleCount++;
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                draggingIndex = -1;
            }

            // mód váltás
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_1) drawMode = 0;
                if (event.key.keysym.sym == SDLK_2) drawMode = 1;
                if (event.key.keysym.sym == SDLK_3) drawMode = 2;
            }
        }

        // ---- Rajzolás ----

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        // körök
        for (int i = 0; i < circleCount; i++) {

            SDL_SetRenderDrawColor(renderer,
                circles[i].color.r,
                circles[i].color.g,
                circles[i].color.b,
                255);

            if (drawMode == 0)
                drawCircle_steps(renderer, circles[i], 40);
            else if (drawMode == 1)
                drawCircle_angle(renderer, circles[i], 0.2);
            else
                drawCircle_length(renderer, circles[i], 5.0);

            // hover jel
            if (isMouseOverCircle(mx, my, circles[i])) {
                SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                drawPlus(renderer, circles[i]);
            }
        }

        // paletta
        for (int i = 0; i < PALETTE_SIZE; i++) {
            SDL_Rect rect = {10 + i*50, 10, 40, 40};

            SDL_SetRenderDrawColor(renderer,
                palette[i].r,
                palette[i].g,
                palette[i].b,
                255);

            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}