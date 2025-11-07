#ifndef SUDOKU_H_
#define SUDOKU_H_

#include <stdint.h>
#include <stdbool.h>

// --- ANSI Color Codes (Not used by Raylib, but harmless) ---
#define ANSI_RESET      "\033[0m"
#define ANSI_BOLD       "\033[1m"
#define ANSI_BLUE       "\033[1;34m"
#define ANSI_RED        "\033[1;31m"

/**
 * @brief Main struct to hold the entire game state.
 */
typedef struct {
    uint8_t board[9][9]; 
    uint16_t row_masks[9];
    uint16_t col_masks[9];
    uint16_t box_masks[9];
    bool initial_board[9][9]; 
    int cells_filled;
} SudokuGame;

/**
 * @brief Initializes the game struct from a 9x9 puzzle array.
 */
void init_game(SudokuGame* game, const uint8_t puzzle[9][9]);

/**
 * @brief (Console only) Displays the current board state to stdout.
 */
void display_board(const SudokuGame* game);

/**
 * @brief Attempts to make a move on the board (O(1) validation).
 */
bool make_move(SudokuGame* game, int r, int c, uint8_t num);

/**
 * @brief Checks if the board is fully and correctly solved.
 */
bool is_board_solved(const SudokuGame* game);

/**
 * @brief Solves the Sudoku puzzle using optimized backtracking.
 */
bool solve_sudoku(SudokuGame* game);

#endif // SUDOKU_H_