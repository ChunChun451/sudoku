#include "sudoku.h"
#include <stdio.h>
#include <string.h>

static int get_box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static bool is_valid(const SudokuGame* game, int r, int c, uint8_t num) {
    const uint16_t bit = 1 << (num - 1);
    const int box_idx = get_box_index(r, c);
    
    return !((game->row_masks[r] & bit) ||
             (game->col_masks[c] & bit) ||
             (game->box_masks[box_idx] & bit));
}

static void set_cell(SudokuGame* game, int r, int c, uint8_t num, bool is_initial) {
    game->board[r][c] = num;
    game->initial_board[r][c] = is_initial;

    const uint16_t bit = 1 << (num - 1);
    const int box_idx = get_box_index(r, c);

    game->row_masks[r] |= bit;
    game->col_masks[c] |= bit;
    game->box_masks[box_idx] |= bit;

    game->cells_filled++;
}

static void clear_cell(SudokuGame* game, int r, int c) {
    if (game->board[r][c] == 0) {
        return; // Already empty
    }

    const uint8_t num = game->board[r][c];
    const uint16_t bit = 1 << (num - 1);
    const int box_idx = get_box_index(r, c);

    game->row_masks[r] &= ~bit;
    game->col_masks[c] &= ~bit;
    game->box_masks[box_idx] &= ~bit;

    game->board[r][c] = 0;
    game->initial_board[r][c] = false; 
    game->cells_filled--;
}

static int count_set_bits(uint16_t n) {
    int count = 0;
    while (n > 0) {
        n &= (n - 1);
        count++;
    }
    return count;
}

/**
 * @brief OPTIMIZATION: Finds the empty cell with the Minimum Remaining Values (MRV).
 */
static bool find_best_empty_cell(SudokuGame* game, int* best_r, int* best_c) {
    int min_options = 10;
    bool found = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->board[r][c] == 0) {
                const int box_idx = get_box_index(r, c);
                const uint16_t used_mask = game->row_masks[r] | 
                                           game->col_masks[c] | 
                                           game->box_masks[box_idx];
                const uint16_t available_mask = (~used_mask) & 0x1FF;
                int options = count_set_bits(available_mask);

                if (options < min_options) {
                    min_options = options;
                    *best_r = r;
                    *best_c = c;
                    found = true;
                }
                
                if (min_options <= 1) {
                    return true;
                }
            }
        }
    }
    return found;
}

static bool solve_recursive(SudokuGame* game) {
    int r, c;
    
    if (!find_best_empty_cell(game, &r, &c)) {
        return true; 
    }

    const int box_idx = get_box_index(r, c);
    const uint16_t used_mask = game->row_masks[r] | game->col_masks[c] | game->box_masks[box_idx];
    const uint16_t available_mask = (~used_mask) & 0x1FF;
    
    for (uint8_t num = 1; num <= 9; num++) {
        if ((available_mask >> (num - 1)) & 1) {
            set_cell(game, r, c, num, false);
            if (solve_recursive(game)) {
                return true;
            }
            clear_cell(game, r, c);
        }
    }
    return false;
}

void init_game(SudokuGame* game, const uint8_t puzzle[9][9]) {
    memset(game, 0, sizeof(SudokuGame));

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            uint8_t num = puzzle[r][c];
            if (num >= 1 && num <= 9) {
                set_cell(game, r, c, num, true);
            }
        }
    }
}

bool make_move(SudokuGame* game, int r, int c, uint8_t num) {
    if (r < 0 || r > 8 || c < 0 || c > 8 || num > 9) {
        return false; 
    }

    if (game->initial_board[r][c]) {
        return false;
    }

    if (game->board[r][c] != 0) {
        clear_cell(game, r, c);
    }
    
    if (num == 0) {
        return true; // Clear was successful
    }

    if (is_valid(game, r, c, num)) {
        set_cell(game, r, c, num, false);
        return true;
    }
    
    return false; // Invalid move
}

bool is_board_solved(const SudokuGame* game) {
    if (game->cells_filled != 81) {
        return false;
    }

    const uint16_t full_mask = (1 << 9) - 1; // 511

    for (int i = 0; i < 9; i++) {
        if (game->row_masks[i] != full_mask ||
            game->col_masks[i] != full_mask ||
            game->box_masks[i] != full_mask) {
            return false;
        }
    }
    
    return true;
}

bool solve_sudoku(SudokuGame* game) {
    SudokuGame temp_game;
    uint8_t initial_puzzle[9][9];
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (game->initial_board[r][c]) {
                initial_puzzle[r][c] = game->board[r][c];
            } else {
                initial_puzzle[r][c] = 0;
            }
        }
    }
    
    init_game(&temp_game, initial_puzzle);

    if (solve_recursive(&temp_game)) {
        memcpy(game, &temp_game, sizeof(SudokuGame));
        return true;
    }
    
    return false; 
}