#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>
#include <stdbool.h>

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 20

typedef struct {
    uint8_t  playfield[BOARD_HEIGHT][BOARD_WIDTH];
    int8_t   piece_x;
    int8_t   piece_y;
    uint8_t  current_orientation;
    uint32_t score;
    uint32_t lines;
    uint8_t  level;
    bool     game_over;
} TetrisGame;

void Tetris_Init(TetrisGame *game);
bool Tetris_CheckCollision(const TetrisGame *game, int8_t check_x, int8_t check_y, uint8_t orientation);
void Tetris_Rotate(TetrisGame *game);
void Tetris_MoveLeft(TetrisGame *game);
void Tetris_MoveRight(TetrisGame *game);
bool Tetris_StepGravity(TetrisGame *game);
void Tetris_DropHard(TetrisGame *game);
int  Tetris_ClearLines(TetrisGame *game);

#endif