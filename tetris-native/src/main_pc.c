#include <SDL.h>
#include <stdbool.h>
#include <time.h>
#include "tetris.h"
#include "pieces.h"

#define TILE_SIZE 28
#define WINDOW_WIDTH  (BOARD_WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (BOARD_HEIGHT * TILE_SIZE)

// DMG Game Boy 4-shade green/gray palette
static const SDL_Color PALETTE[8] = {
    { 15,  56,  15, 255 }, // 0: Background
    { 48,  98,  48, 255 }, // 1: Mino Cyan
    { 139, 172, 15, 255 }, // 2: Mino Blue
    { 155, 188, 15, 255 }, // 3: Mino Orange
    { 100, 140, 20, 255 }, // 4: Mino Yellow
    {  80, 120, 30, 255 }, // 5: Mino Green
    { 120, 150, 40, 255 }, // 6: Mino Purple
    {  60,  90, 20, 255 }  // 7: Mino Red
};

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    srand((unsigned int)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Tetris Native C Port (Decompiled SM83)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TetrisGame game;
    Tetris_Init(&game);

    uint32_t last_drop_time = SDL_GetTicks();
    uint32_t drop_interval = 500; // Gravity step (ms)
    bool running = true;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:   Tetris_MoveLeft(&game); break;
                    case SDLK_RIGHT:  Tetris_MoveRight(&game); break;
                    case SDLK_UP:
                    case SDLK_x:      Tetris_Rotate(&game); break;
                    case SDLK_DOWN:   Tetris_StepGravity(&game); break;
                    case SDLK_SPACE:  Tetris_DropHard(&game); break;
                    case SDLK_r:      Tetris_Init(&game); break;
                    case SDLK_ESCAPE: running = false; break;
                }
            }
        }

        // Gravity update
        uint32_t now = SDL_GetTicks();
        if (now - last_drop_time > drop_interval) {
            Tetris_StepGravity(&game);
            last_drop_time = now;
        }

        // --- Render Frame ---
        // 1. Draw Background
        SDL_SetRenderDrawColor(renderer, PALETTE[0].r, PALETTE[0].g, PALETTE[0].b, 255);
        SDL_RenderClear(renderer);

        // 2. Draw Locked Playfield Minos
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                uint8_t tile = game.playfield[y][x];
                if (tile > 0) {
                    SDL_SetRenderDrawColor(renderer, PALETTE[tile].r, PALETTE[tile].g, PALETTE[tile].b, 255);
                    SDL_Rect r = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE - 1, TILE_SIZE - 1 };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        // 3. Draw Active Falling Piece
        if (!game.game_over) {
            const PieceOrientation *p = &TETROMINO_TABLE[game.current_orientation];
            SDL_SetRenderDrawColor(renderer, PALETTE[p->color_id].r, PALETTE[p->color_id].g, PALETTE[p->color_id].b, 255);
            for (int i = 0; i < 4; i++) {
                int tx = game.piece_x + p->blocks[i].x;
                int ty = game.piece_y + p->blocks[i].y;
                if (ty >= 0) {
                    SDL_Rect r = { tx * TILE_SIZE, ty * TILE_SIZE, TILE_SIZE - 1, TILE_SIZE - 1 };
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Cap near 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}