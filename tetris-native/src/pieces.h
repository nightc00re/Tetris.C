#ifndef PIECES_H
#define PIECES_H

#include <stdint.h>

typedef struct {
    int8_t y;
    int8_t x;
} BlockOffset;

typedef struct {
    BlockOffset blocks[4];
    uint8_t next_rotation; // Next orientation index on rotate
    uint8_t color_id;      // Visual tile ID
} PieceOrientation;

// 19 total orientations across the 7 tetromino types
static const PieceOrientation TETROMINO_TABLE[19] = {
    // --- I-Piece (0..1) ---
    { { {0, 0}, {0, 1}, {0, 2}, {0, 3} }, 1, 1 }, // Horiz
    { { {0, 1}, {1, 1}, {2, 1}, {3, 1} }, 0, 1 }, // Vert

    // --- J-Piece (2..5) ---
    { { {0, 0}, {1, 0}, {1, 1}, {1, 2} }, 3, 2 },
    { { {0, 1}, {1, 1}, {2, 1}, {2, 0} }, 4, 2 },
    { { {0, 0}, {0, 1}, {0, 2}, {1, 2} }, 5, 2 },
    { { {0, 1}, {0, 2}, {1, 1}, {2, 1} }, 2, 2 },

    // --- L-Piece (6..9) ---
    { { {0, 2}, {1, 0}, {1, 1}, {1, 2} }, 7, 3 },
    { { {0, 0}, {1, 0}, {2, 0}, {2, 1} }, 8, 3 },
    { { {0, 0}, {0, 1}, {0, 2}, {1, 0} }, 9, 3 },
    { { {0, 0}, {0, 1}, {1, 1}, {2, 1} }, 6, 3 },

    // --- O-Piece (10) ---
    { { {0, 0}, {0, 1}, {1, 0}, {1, 1} }, 10, 4 },

    // --- Z-Piece (11..12) ---
    { { {0, 0}, {0, 1}, {1, 1}, {1, 2} }, 12, 5 },
    { { {0, 1}, {1, 0}, {1, 1}, {2, 0} }, 11, 5 },

    // --- T-Piece (13..16) ---
    { { {0, 1}, {1, 0}, {1, 1}, {1, 2} }, 14, 6 },
    { { {0, 1}, {1, 1}, {1, 2}, {2, 1} }, 15, 6 },
    { { {0, 0}, {0, 1}, {0, 2}, {1, 1} }, 16, 6 },
    { { {0, 1}, {1, 0}, {1, 1}, {2, 1} }, 13, 6 },

    // --- S-Piece (17..18) ---
    { { {0, 1}, {0, 2}, {1, 0}, {1, 1} }, 18, 7 },
    { { {0, 0}, {1, 0}, {1, 1}, {2, 1} }, 17, 7 }
};

// Starting orientation index for each of the 7 pieces
static const uint8_t PIECE_SPAWN_INDEX[7] = {0, 2, 6, 10, 11, 13, 17};

#endif