/// Author : GhostOne
/// Created Date: 15th April, 2024
/// All the logic code related to this particular sudoku application

#pragma once

#define DOSUKU_API "https://sudoku-api.vercel.app/api/dosuku"
#define SUGOKU_API "API2 HERE"

#include <common.hpp>
#include <iostream>
#include <future>
#include <memory>
#include "RSJparser.tcc"

enum SUDOKU_BOARD_GENERATOR_API
{
    DOSUKU, // https://sudoku-api.vercel.app,
    SUGOKU  // https://github.com/bertoort/sugoku
};

struct CELL_LOCATION
{
    unsigned int X : 4;
    unsigned int Y : 4;
};

/// @brief meta data for a particular cell on the sudoku board
struct Cell
{

    // the position index of the cell on the sudoku board
    CELL_LOCATION location;
    CELL_LOCATION targets[24];

    // the value inside the particular cell
    unsigned int value : 4;
};

class SudokuBoardGenerator;
class SudokuBoard;
class DOSUKU_SudokuBoardGenerator;
class SUGOKU_SudokuBoardGenerator;

static SudokuBoardGenerator *generateBoardGenerator(SudokuBoard &targetBoard, SUDOKU_BOARD_GENERATOR_API type = DOSUKU);

class SudokuBoardGenerator
{

    std::string ResponseString;

protected:
    Event BoardGeneratedEvent;
    Event SuccessResponseEvent;
    Event BoardGenerationFailedEvent;

    flecs::world *board;

    SudokuBoardGenerator(SudokuBoard &board)
    {

        AddEventHandler(SuccessResponseEvent, [&](std::string returnContent)
                        { generateBoard(std::string(returnContent)); });

        AddEventHandler(BoardGeneratedEvent, [](std::string returnContent) {

        });

        AddEventHandler(BoardGenerationFailedEvent, [](std::string returnString)
                        { std::cout << "Board Generation Failed Successfully" << std::endl; });
    }

    void sendRequest(std::string api_string)
    {
        WebRequestClient client = CreateWebRequestClient(api_string.c_str(), GET);

        AddWebRequestSuccessEventHandler(client, [this](std::string returnContent)
                                         { EmitEvent(SuccessResponseEvent, returnContent); });
        AddWebRequestFailedEventHandler(client, [this](std::string returnContent)
                                        { EmitEvent(BoardGenerationFailedEvent, returnContent); });

        SendWebRequest(client);
    }

    void OnBoardGenerated()
    {
    }

public:
    SUDOKU_BOARD_GENERATOR_API type;

    virtual void generateBoard(std::string boardJson) = 0;

    void startGenerateBoard()
    {
        if (type == DOSUKU)
        {
            sendRequest(DOSUKU_API);
        }
        else
        {
            sendRequest(SUGOKU_API);
        }
    }
};

class SudokuBoard
{

    flecs::world *cellsOnBoard;
    SudokuBoardGenerator *generator;

public:
    void setBoard(flecs::world &newBoard)
    {
        delete cellsOnBoard;
        cellsOnBoard = &newBoard;
    }

    void loadSudoku()
    {
        this->generator->startGenerateBoard();
    };

    SudokuBoard(SUDOKU_BOARD_GENERATOR_API type = DOSUKU)
    {
        // cellsOnBoard = std::make_unique<flecs::world>();
        this->generator = generateBoardGenerator(*this, type);
        std::cout << "Generator Type: " << generator->type << std::endl;
    }
};

class DOSUKU_SudokuBoardGenerator : public SudokuBoardGenerator
{

public:
    DOSUKU_SudokuBoardGenerator(SudokuBoard targetBoard) : SudokuBoardGenerator(targetBoard)
    {
        this->type = DOSUKU;
    }

    void generateBoard(std::string boardJson)
    {
        RSJresource my_json(boardJson);
        std::cout << my_json["newboard"]["grids"][0]["value"].size() << std::endl;
    }
};

class SUGOKU_SudokuBoardGenerator : public SudokuBoardGenerator
{
public:
    SUGOKU_SudokuBoardGenerator(SudokuBoard targetBoard) : SudokuBoardGenerator(targetBoard)
    {
        this->type = SUGOKU;
    }

    void generateBoard(std::string boardJson)
    {
        std::cout << boardJson << std::endl;
    }
};

static SudokuBoardGenerator *generateBoardGenerator(SudokuBoard &targetBoard, SUDOKU_BOARD_GENERATOR_API type)
{
    switch (type)
    {
    case DOSUKU:
        return new DOSUKU_SudokuBoardGenerator(targetBoard);
    case SUGOKU:
        return new SUGOKU_SudokuBoardGenerator(targetBoard);
    default:
        return generateBoardGenerator(targetBoard);
    }
}