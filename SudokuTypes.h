#pragma once
#include<array>

//Classic 9x9 sudoku
static constexpr int nonetwidth = 3;
static constexpr int sudokuwidth = nonetwidth * nonetwidth;
static constexpr int arraylength = sudokuwidth * sudokuwidth;

using SudokuArray = std::array<int, arraylength>;
using SudokuWidthArray = std::array<int, sudokuwidth>;
