#include "SudokuSolver.h"
#include "array1465.h"
int main(int argc, char* argv[])
{
	int index = 0;
	for (auto& puzzle : top1465)
	{
		SudokuSolver(puzzle);
		printf("%d\n", index++);

	}

}