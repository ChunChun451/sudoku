/**********************************************************************************
 *
 * Raylib Sudoku - Main UI File
 *
 * This file handles all graphics, input, and game state.
 * It uses the modular sudoku logic from sudoku.c and sudoku.h.
 *
 **********************************************************************************/

#include "raylib.h"
#include "sudoku.h" // <-- Includes our game logic!
#include <stdio.h>    // For sprintf
#include <stdlib.h>   // For rand()
#include <time.h>     // For time()
#include <string.h>   // For strcpy/strncmp

// --- NEW: A cleaner, warmer color palette ---
#define BG_WARM_WHITE   (Color){ 250, 248, 247, 255 } // A soft, warm off-white
#define GRID_THICK_COLOR (Color){ 110, 110, 110, 255 } // A dark gray
#define GRID_THIN_COLOR  (Color){ 200, 200, 200, 255 } // A light gray
#define TITLE_COLOR      (Color){ 80, 80, 80, 255 }    // Darker gray for title
#define INITIAL_NUM_COLOR (Color){ 50, 50, 50, 255 }    // Near-black for initial numbers
#define USER_NUM_COLOR   (Color){ 0, 121, 241, 255 }  // A pleasant "UI blue"
#define HIGHLIGHT_COLOR  (Color){ 255, 215, 0, 100 }  // A transparent gold
#define ERROR_COLOR      (Color){ 230, 41, 55, 255 }    // Standard UI Red
#define STATUS_COLOR     (Color){ 100, 100, 100, 255 }  // Gray for status messages
#define SOLVED_COLOR     (Color){ 0, 150, 80, 255 }    // A nice green

// --- Puzzle Bank (Unchanged) ---
#define NUM_PUZZLES 3
const uint8_t puzzle_bank[NUM_PUZZLES][9][9] = {
    // Puzzle 0: Easy
    {
        {5, 3, 0, 0, 7, 0, 0, 0, 0}, {6, 0, 0, 1, 9, 5, 0, 0, 0}, {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3}, {4, 0, 0, 8, 0, 3, 0, 0, 1}, {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0}, {0, 0, 0, 4, 1, 9, 0, 0, 5}, {0, 0, 0, 0, 8, 0, 0, 7, 9}
    },
    // Puzzle 1: Medium
    {
        {0, 2, 0, 6, 0, 8, 0, 0, 0}, {5, 8, 0, 0, 0, 9, 7, 0, 0}, {0, 0, 0, 0, 4, 0, 0, 0, 0},
        {3, 7, 0, 0, 0, 0, 5, 0, 0}, {6, 0, 0, 0, 0, 0, 0, 0, 4}, {0, 0, 8, 0, 0, 0, 0, 1, 3},
        {0, 0, 0, 0, 2, 0, 0, 0, 0}, {0, 0, 9, 8, 0, 0, 0, 3, 6}, {0, 0, 0, 3, 0, 6, 0, 9, 0}
    },
    // Puzzle 2: Hard
    {
        {8, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 3, 6, 0, 0, 0, 0, 0}, {0, 7, 0, 0, 9, 0, 2, 0, 0},
        {0, 5, 0, 0, 0, 7, 0, 0, 0}, {0, 0, 0, 0, 4, 5, 7, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 3, 0},
        {0, 0, 1, 0, 0, 0, 0, 6, 8}, {0, 0, 8, 5, 0, 0, 0, 1, 0}, {0, 9, 0, 0, 0, 0, 4, 0, 0}
    }
};

// --- UI Constants ---
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 700
#define GRID_SIZE 540
#define CELL_SIZE (GRID_SIZE / 9)
#define GRID_START_X ((SCREEN_WIDTH - GRID_SIZE) / 2)
#define GRID_START_Y 50
#define NUMBER_FONT_SIZE 40
#define TITLE_FONT_SIZE 30
#define TEXT_FONT_SIZE 20

// --- Main Function ---
int main(void) {
    // --- Initialization ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sudoku - Raylib");
    srand(time(NULL));

    SudokuGame game;
    char message[256] = "Click a cell. Press 'S' to solve, 'N' for new game.";
    
    int selectedRow = -1;
    int selectedCol = -1;
    bool solvedState = false;
    
    init_game(&game, puzzle_bank[0]);

    SetTargetFPS(60);

    // --- Main Game Loop ---
    while (!WindowShouldClose()) {
        
        // --- Input & Update Logic ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            int x = mousePos.x;
            int y = mousePos.y;

            if (x >= GRID_START_X && x < (GRID_START_X + GRID_SIZE) &&
                y >= GRID_START_Y && y < (GRID_START_Y + GRID_SIZE)) {
                selectedCol = (x - GRID_START_X) / CELL_SIZE;
                selectedRow = (y - GRID_START_Y) / CELL_SIZE;
                strcpy(message, "");
            } else {
                selectedRow = -1;
                selectedCol = -1;
            }
        }

        if (selectedRow != -1 && !solvedState) {
            int keyPressed = GetKeyPressed();

            if (keyPressed >= KEY_ONE && keyPressed <= KEY_NINE) {
                int num = keyPressed - KEY_ONE + 1;
                if (!make_move(&game, selectedRow, selectedCol, num)) {
                    if (game.initial_board[selectedRow][selectedCol]) {
                        strcpy(message, "Cannot change a fixed cell.");
                    } else {
                        sprintf(message, "Invalid move: %d conflicts.", num);
                    }
                } else {
                    strcpy(message, "");
                }
            }

            if (keyPressed == KEY_ZERO || keyPressed == KEY_BACKSPACE || keyPressed == KEY_DELETE) {
                if (!make_move(&game, selectedRow, selectedCol, 0)) {
                     strcpy(message, "Cannot change a fixed cell.");
                } else {
                    strcpy(message, "Cell cleared.");
                }
            }
        }

        int commandKey = GetKeyPressed();
        if (commandKey == KEY_S) {
            if (solve_sudoku(&game)) {
                strcpy(message, "Puzzle Solved!");
                solvedState = true;
            } else {
                strcpy(message, "Error: This puzzle is unsolvable.");
            }
        }
        
        if (commandKey == KEY_N) {
            init_game(&game, puzzle_bank[rand() % NUM_PUZZLES]);
            solvedState = false;
            selectedRow = -1;
            selectedCol = -1;
            strcpy(message, "New game started.");
        }

        if (!solvedState && is_board_solved(&game)) {
            solvedState = true;
            strcpy(message, "Congratulations! You solved the puzzle!");
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BG_WARM_WHITE);
        
        // Draw Title (Centered)
        int titleWidth = MeasureText("Sudoku", TITLE_FONT_SIZE);
        DrawText("Sudoku", (SCREEN_WIDTH - titleWidth) / 2, 10, TITLE_FONT_SIZE, TITLE_COLOR);

        // Draw Grid Lines (Softer Colors)
        for (int i = 0; i <= 9; i++) {
            float thickness = (i % 3 == 0) ? 3.0f : 1.5f;
            Color lineColor = (i % 3 == 0) ? GRID_THICK_COLOR : GRID_THIN_COLOR;
            DrawLineEx((Vector2){GRID_START_X, GRID_START_Y + i * CELL_SIZE}, (Vector2){GRID_START_X + GRID_SIZE, GRID_START_Y + i * CELL_SIZE}, thickness, lineColor);
            DrawLineEx((Vector2){GRID_START_X + i * CELL_SIZE, GRID_START_Y}, (Vector2){GRID_START_X + i * CELL_SIZE, GRID_START_Y + GRID_SIZE}, thickness, lineColor);
        }

        // Draw Selected Cell Highlight (Modern)
        if (selectedRow != -1 && selectedCol != -1) {
            DrawRectangle(GRID_START_X + selectedCol * CELL_SIZE, GRID_START_Y + selectedRow * CELL_SIZE, CELL_SIZE, CELL_SIZE, HIGHLIGHT_COLOR);
        }

        // Draw Numbers (Centered & Colored)
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (game.board[i][j] != 0) {
                    char numStr[2];
                    sprintf(numStr, "%d", game.board[i][j]);
                    int textWidth = MeasureText(numStr, NUMBER_FONT_SIZE);
                    
                    // Center text perfectly
                    int xPos = GRID_START_X + (j * CELL_SIZE) + (CELL_SIZE - textWidth) / 2;
                    int yPos = GRID_START_Y + (i * CELL_SIZE) + (CELL_SIZE - NUMBER_FONT_SIZE) / 2;

                    Color numColor = game.initial_board[i][j] ? INITIAL_NUM_COLOR : USER_NUM_COLOR;
                    DrawText(numStr, xPos, yPos, NUMBER_FONT_SIZE, numColor);
                }
            }
        }

        // Draw Message/Instructions (Color Coded)
        Color msgColor = STATUS_COLOR;
        if (strncmp(message, "Error", 5) == 0 || strncmp(message, "Invalid", 7) == 0 || strncmp(message, "Cannot", 6) == 0) {
            msgColor = ERROR_COLOR;
        } else if (strncmp(message, "Puzzle Solved", 13) == 0 || strncmp(message, "Congratulations", 15) == 0) {
            msgColor = SOLVED_COLOR;
        }
        
        int msgWidth = MeasureText(message, TEXT_FONT_SIZE);
        DrawText(message, (SCREEN_WIDTH - msgWidth) / 2, GRID_START_Y + GRID_SIZE + 20, TEXT_FONT_SIZE, msgColor);
        
        // Draw Footer
        const char* footerText = "Press 'S'-Solve | 'N'-New Game | '0/DEL'-Clear";
        int footerWidth = MeasureText(footerText, TEXT_FONT_SIZE - 2); // Slightly smaller
        
        if (solvedState) {
            footerText = "GAME SOLVED! Press 'N' for a new game.";
            footerWidth = MeasureText(footerText, TEXT_FONT_SIZE - 2);
            DrawText(footerText, (SCREEN_WIDTH - footerWidth) / 2, SCREEN_HEIGHT - 30, TEXT_FONT_SIZE - 2, SOLVED_COLOR);
        } else {
            DrawText(footerText, (SCREEN_WIDTH - footerWidth) / 2, SCREEN_HEIGHT - 30, TEXT_FONT_SIZE - 2, GRAY);
        }

        EndDrawing();
    }

    // --- Cleanup ---
    CloseWindow();
    return 0;
}