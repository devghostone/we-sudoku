#define RAYGUI_IMPLEMENTATION

#define PROJECT_NAME "We Sudoku"
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 525

#define CELL_SIZE 45
#define CELL_MARGIN 16

#include "common.hpp"
#include "sudoku-core.hpp"
#include "memory"
#include "web-requests.hpp"

Camera2D camera = { 0 };

void cameraSetup(){
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void DrawGrid(int grid_size)
{
    // Calculate the grid cell size based on the window size
    int cell_size = SCREEN_WIDTH / (grid_size + 2);

    // Calculate the starting position to center the grid
    int start_x = (SCREEN_HEIGHT - (grid_size * cell_size)) / 2;
    int start_y = (SCREEN_HEIGHT - (grid_size * cell_size)) / 2;

    // Draw the grid
    for (int x = 0; x <= grid_size; x++)
    {   
        Color color = x % 3  == 0? BLACK: LIGHTGRAY;
        DrawLine(start_x + x * cell_size, start_y, start_x + x * cell_size, start_y + grid_size * cell_size, color);
    }

    for (int y = 0; y <= grid_size; y++)
    {
        Color color = y % 3  == 0? BLACK: LIGHTGRAY;
        DrawLine(start_x, start_y + y * cell_size, start_x + grid_size * cell_size, start_y + y * cell_size, color);
    }
}

void DrawSudokuBoard(SudokuBoard& board, int grid_size){

    if(board.cellsOnBoard == nullptr){
        return;
    }

    board.cellsOnBoard->system<Cell>().iter(
        [&](flecs::iter it, Cell* cell){

            int cell_size = SCREEN_WIDTH / (11);

            int start_x = (SCREEN_HEIGHT - (9 * CELL_SIZE)) / 2;
            int start_y = (SCREEN_HEIGHT - (9 * CELL_SIZE)) / 2;

            for(auto i: it){
                const char* value = std::to_string(cell[i].value).c_str();
                //std::cout << "X: " << cell[i].location.X << ", Y: " << cell[i].location.Y << ", value: " << value << ", Cell Size: " << cell_size << "\n";
                DrawText(value, start_x + CELL_SIZE * (cell[i].location.X - 1) + CELL_MARGIN, start_y + CELL_SIZE * (cell[i].location.Y - 1) + CELL_MARGIN, 20, BLACK);
            }
        });
}

int main(){

    InitWindow(SCREEN_WIDTH, SCREEN_WIDTH, PROJECT_NAME);
    InitAudioDevice();
    InitWebRequestService();
    cameraSetup();

    std::unique_ptr<SudokuBoard> board = std::make_unique<SudokuBoard>(DOSUKU);
    board->loadSudoku();

    while(!WindowShouldClose()){
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        
        DrawGrid(9);
        board->cellsOnBoard->progress();
        DrawSudokuBoard(*board, 9);

        EndMode2D();
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}