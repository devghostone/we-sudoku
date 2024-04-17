#define RAYGUI_IMPLEMENTATION

#define PROJECT_NAME "We Sudoku"
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 525

#include "common.hpp"
#include "sudoku-core.hpp"
#include "memory"
#include "web-requests.hpp"

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_WIDTH, PROJECT_NAME);
    InitAudioDevice();
    InitWebRequestService();

    std::unique_ptr<SudokuBoard> board = std::make_unique<SudokuBoard>(DOSUKU);
    board->loadSudoku();

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}