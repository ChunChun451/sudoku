#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t board[9][9]; 
    uint16_t row_masks[9];
    uint16_t col_masks[9];
    uint16_t box_masks[9];
    bool initial_board[9][9]; 
    int cells_filled;
} SudokuGame;

void init_game(SudokuGame* game, const uint8_t puzzle[9][9]);

bool make_move(SudokuGame* game, int r, int c, uint8_t num);

bool is_board_solved(const SudokuGame* game);

bool solve_sudoku(SudokuGame* game);

#endif