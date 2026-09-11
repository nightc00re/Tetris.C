#include "tetris.h"
#include "pieces.h"
#include <stdlib.h>
#include <string.h>

static void SpawnPiece(TetrisGame *game) {
    uint8_t type = rand() % 7;
    game->current_orientation = PIECE_SPAWN_INDEX[type];
    game->piece_x = 3;
    game->piece_y = 0;

    // Speculative collision check on spawn (GameOver condition)
    if (Tetris_CheckCollision(game, game->piece_x, game->piece_y, game->current_orientation)) {
        game->game_over = true;
    }
}

void Tetris_Init(TetrisGame *game) {
    memset(game->playfield, 0, sizeof(game->playfield));
    game->score = 0;
    game->lines = 0;
    game->level = 0;
    game->game_over = false;
    SpawnPiece(game);
}

// Speculative collision validation routine from 0x1AC0
bool Tetris_CheckCollision(const TetrisGame *game, int8_t check_x, int8_t check_y, uint8_t orientation) {
    const PieceOrientation *p = &TETROMINO_TABLE[orientation];
    for (int i = 0; i < 4; i++) {
        int tx = check_x + p->blocks[i].x;
        int ty = check_y + p->blocks[i].y;

        if (tx < 0 || tx >= BOARD_WIDTH) return true;  // Wall collision
        if (ty >= BOARD_HEIGHT)          return true;  // Floor collision
        if (ty >= 0 && game->playfield[ty][tx] != 0) {
            return true;                               // Locked mino collision
        }
    }
    return false;
}

void Tetris_Rotate(TetrisGame *game) {
    uint8_t next = TETROMINO_TABLE[game->current_orientation].next_rotation;
    if (!Tetris_CheckCollision(game, game->piece_x, game->piece_y, next)) {
        game->current_orientation = next;
    }
}

void Tetris_MoveLeft(TetrisGame *game) {
    if (!Tetris_CheckCollision(game, game->piece_x - 1, game->piece_y, game->current_orientation)) {
        game->piece_x--;
    }
}

void Tetris_MoveRight(TetrisGame *game) {
    if (!Tetris_CheckCollision(game, game->piece_x + 1, game->piece_y, game->current_orientation)) {
        game->piece_x++;
    }
}

static void LockPiece(TetrisGame *game) {
    const PieceOrientation *p = &TETROMINO_TABLE[game->current_orientation];
    for (int i = 0; i < 4; i++) {
        int tx = game->piece_x + p->blocks[i].x;
        int ty = game->piece_y + p->blocks[i].y;
        if (ty >= 0 && ty < BOARD_HEIGHT && tx >= 0 && tx < BOARD_WIDTH) {
            game->playfield[ty][tx] = p->color_id;
        }
    }
    Tetris_ClearLines(game);
    SpawnPiece(game);
}

// Memory line-clearing & row-collapse from 0x1D20
int Tetris_ClearLines(TetrisGame *game) {
    int cleared = 0;

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->playfield[y][x] == 0) {
                full = false;
                break;
            }
        }

        if (full) {
            cleared++;
            // Shift all rows above down by one
            for (int sy = y; sy > 0; sy--) {
                memcpy(game->playfield[sy], game->playfield[sy - 1], BOARD_WIDTH);
            }
            // Zero out top row
            memset(game->playfield[0], 0, BOARD_WIDTH);
            y++; // Re-evaluate current row index
        }
    }

    if (cleared > 0) {
        static const uint16_t BASE_POINTS[5] = {0, 40, 100, 300, 1200};
        game->score += BASE_POINTS[cleared] * (game->level + 1);
        game->lines += cleared;
        game->level = game->lines / 10;
    }
    return cleared;
}

bool Tetris_StepGravity(TetrisGame *game) {
    if (game->game_over) return false;

    if (!Tetris_CheckCollision(game, game->piece_x, game->piece_y + 1, game->current_orientation)) {
        game->piece_y++;
        return true;
    } else {
        LockPiece(game);
        return false;
    }
}

void Tetris_DropHard(TetrisGame *game) {
    while (!Tetris_CheckCollision(game, game->piece_x, game->piece_y + 1, game->current_orientation)) {
        game->piece_y++;
    }
    LockPiece(game);
}