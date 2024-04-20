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

constexpr int START_POS = (SCREEN_HEIGHT - (9 * CELL_SIZE)) / 2;
constexpr int END_POS = START_POS + CELL_SIZE * 9;

Camera2D camera = { 0 };
CELL_LOCATION input_location = {0, 0};

///#region input
/// @brief determine the X/Y input location from Screen X/Y
CELL_LOCATION get_input_cell_location(Camera2D camera){
    Vector2 mousePosition = GetScreenToWorld2D(GetMousePosition(), camera);
    CELL_LOCATION location;
    if(mousePosition.x < START_POS || mousePosition.x > END_POS || mousePosition.y < START_POS || mousePosition.y > END_POS){
        location.X = 0;
        location.Y = 0;
        return location;
    }
    location.X = (int)(mousePosition.x - START_POS) / CELL_SIZE + 1;
    location.Y = (int)(mousePosition.y - START_POS) / CELL_SIZE + 1;
    return location;
}
void add_to_board(SudokuBoard& board);
///#endregion

void cameraSetup(){
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 0, 0 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void DrawGrid(int grid_size)
{
    for (int x = 0; x <= grid_size; x++)
    {   
        Color color = x % 3  == 0? BLACK: LIGHTGRAY;
        DrawLine(START_POS + x * CELL_SIZE, START_POS, START_POS + x * CELL_SIZE, START_POS + grid_size * CELL_SIZE, color);
    }

    for (int y = 0; y <= grid_size; y++)
    {
        Color color = y % 3  == 0? BLACK: LIGHTGRAY;
        DrawLine(START_POS, START_POS + y * CELL_SIZE, START_POS + grid_size * CELL_SIZE, START_POS + y * CELL_SIZE, color);
    }
}

void DrawSudokuBoard(SudokuBoard& board){

    if(board.cellsOnBoard == nullptr){
        return;
    }

    board.cellsOnBoard->system<Cell>().iter(
        [&](flecs::iter it, Cell* cell){

            for(auto i: it){
                const char* value = std::to_string(cell[i].value).c_str();
                //std::cout << "X: " << cell[i].location.X << ", Y: " << cell[i].location.Y << ", value: " << value << ", Cell Size: " << cell_size << "\n";
                DrawText(value, START_POS + CELL_SIZE * (cell[i].location.X - 1) + CELL_MARGIN, START_POS + CELL_SIZE * (cell[i].location.Y - 1) + CELL_MARGIN, 20, BLACK);
            }
        });
}

void DrawMouseRectangle(Camera2D camera){
    CELL_LOCATION location = get_input_cell_location(camera);
    if(location.X == 0 || location.Y == 0){
        return;
    }
    DrawRectangle(START_POS + (location.X - 1) * CELL_SIZE, START_POS + (location.Y - 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLUE);
}

void DrawInputRectangle(){
    if(input_location.X == 0 || input_location.Y == 0){
        return;
    }
    DrawRectangle(START_POS + (input_location.X - 1) * CELL_SIZE, START_POS + (input_location.Y - 1) * CELL_SIZE, CELL_SIZE, CELL_SIZE, YELLOW);
}

void SetInputRectangle(){
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Vector2 mousePosition = GetMousePosition();
        if(mousePosition.x < START_POS || mousePosition.x > END_POS || mousePosition.y < START_POS || mousePosition.y > END_POS){
            input_location.X = 0;
            input_location.Y = 0;
        }
        input_location.X = (int)(mousePosition.x - START_POS) / CELL_SIZE + 1;
        input_location.Y = (int)(mousePosition.y - START_POS) / CELL_SIZE + 1;
    }
}

int GetNumberInput(){
    if(input_location.X == 0 || input_location.X == 0){
        return -1;
    }
    if(IsKeyPressed(KEY_ONE)){
        return 1;
    }
    if(IsKeyPressed(KEY_TWO)){
        return 2;
    }
    if(IsKeyPressed(KEY_THREE)){
        return 3;
    }
    if(IsKeyPressed(KEY_FOUR)){
        return 4;
    }
    if(IsKeyPressed(KEY_FIVE)){
        return 5;
    }
    if(IsKeyPressed(KEY_SIX)){
        return 6;
    }
    if(IsKeyPressed(KEY_SEVEN)){
        return 7;
    }
    if(IsKeyPressed(KEY_EIGHT)){
        return 8;
    }
    if(IsKeyPressed(KEY_NINE)){
        return 9;
    }
    if(IsKeyPressed(KEY_ZERO)){
        return 0;
    }
    return -1;
}

void SetValueInSlot(SudokuBoard& board){

    int input_number = GetNumberInput();
    if(input_number == -1){
        return;
    }
    if(board.cellsOnBoard == nullptr){
        return;
    }

    board.cellsOnBoard->system<Cell>().iter(
        [&](flecs::iter it, Cell* cell){
            int input_number = GetNumberInput();
            if(input_number == -1){
                return;
            }
            for(auto i: it){
                if(cell[i].location.X == input_location.X && cell[i].location.Y == input_location.Y){
                    cell[i].value = input_number;
                    return;
                }
            }
            board.AddValue(input_location.X - 1, input_location.Y - 1, input_number);
        });
}

int main(){

    InitWindow(SCREEN_WIDTH, SCREEN_WIDTH, PROJECT_NAME);
    InitAudioDevice();
    InitWebRequestService();
    SetTargetFPS(60);
    cameraSetup();

    std::unique_ptr<SudokuBoard> board = std::make_unique<SudokuBoard>(DOSUKU);
    board->loadSudoku();

    while(!WindowShouldClose()){
        
        SetInputRectangle();
        SetValueInSlot(*board);

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        
        DrawGrid(9);

        DrawInputRectangle();
        DrawMouseRectangle(camera);

        board->cellsOnBoard->progress();

        DrawSudokuBoard(*board);
        //DrawFPS(10, 10);
        EndMode2D();
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}