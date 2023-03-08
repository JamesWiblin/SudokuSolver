cmake_minimum_required (VERSION 3.1)
set (CMAKE_CXX_STANDARD 20)
project (SudokuSolver)

if(!MSVC)
	message( FATAL_ERROR "Only MSVC is supported at present")
endif()

add_executable (SudokuSolver Main.cpp SudokuSolver.cpp)