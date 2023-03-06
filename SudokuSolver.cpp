#include <string.h>

#ifdef _MSC_VER
#include <intrin.h>
#define __builtin_popcount __popcnt
#pragma intrinsic(_BitScanForward)
#endif

//Classic 9x9 sudoku
static constexpr int nonetwidth = 3;
static constexpr int sudokuwidth = nonetwidth * nonetwidth;
static constexpr int arraylength = sudokuwidth * sudokuwidth;

inline void GetPosition(const int index, int* row, int* column, int* nonet)
{
	*row = index / sudokuwidth;
	*column = index % sudokuwidth;
	*nonet = (*row / nonetwidth) + (nonetwidth * (*column / nonetwidth));
}

int SudokuSolver(int* puzzle)
{
	int i, j;
	int possibleBits[arraylength]; //Contains the possible values for each square in the sudoku as a bit field from index 0-8
	int columnBits[sudokuwidth]; //Contains the currently thought solved values for each column in the sudoku
	int rowBits[sudokuwidth];	//As above but for rows
	int nonetBits[sudokuwidth]; //As above but for nonets
	int bitCurrentlyLegal[arraylength]; //The possible bit we currently believe to be legal
	int firstPossibleBit[arraylength]; //Contains the first possible bit for quick initialisation when backtracking
	int rollingMask[sudokuwidth]; //Used to mask off lower bits when jumping through possibleBits
	int unsolvedSquares = 0;
	int squaresSolved = 0;
	int bitMask = 0;
	int row, column, nonet;
	int rowIndex, columnIndex, nonetIndex;
	int bitField;
	int xBox, yBox;
	int rowOtherBits, columnOtherBits, nonetOtherBits;
	int jump;
	int rowResult, nonetResult, columnResult;
	int nextBitIndex, currentBitIndex, bitToTest;
	bool hiddenSingleFound;
	bool backTracking;

	memset(possibleBits, 0x0, arraylength * sizeof(int));
	memset(columnBits,   0x0, sudokuwidth * sizeof(int));
	memset(rowBits,      0x0, sudokuwidth * sizeof(int));
	memset(nonetBits,    0x0, sudokuwidth * sizeof(int));

	for (j = 0; j < sudokuwidth; j++)
	{
		bitMask <<= 1;
		bitMask |= 1;
		rollingMask[j] = ~bitMask;
	}

	for (j = 0; j < arraylength; j++)
	{
		if (puzzle[j] == sudokuwidth)
		{
			unsolvedSquares++;
			continue;
		}

		possibleBits[j] = 1 << puzzle[j];

		GetPosition(j, &row, &column, &nonet);

		rowBits[row] |= possibleBits[j];
		columnBits[column] |= possibleBits[j];
		nonetBits[nonet] |= possibleBits[j];
	}

	//Loop for deduction solvers
	do
	{
		//'Naked Singles' deduction loop
		do
		{
			squaresSolved = 0;

			for (j = 0; j < arraylength; j++)
			{
				if (__builtin_popcount(possibleBits[j]) != 1)
				{
					bitField = 0;

					GetPosition(j, &row, &column, &nonet);

					//The solved row/column/nonet values in which a square resides gives us the values it CANNOT be
					bitField |= rowBits[row];
					bitField |= columnBits[column];
					bitField |= nonetBits[nonet];

					//Therefore the inverse gives us the values it possibly CAN be
					bitField = ~bitField & bitMask;
					possibleBits[j] = bitField;

					//If there is only 1 possible value it MUST therefore reside at the given square
					if (__builtin_popcount(bitField) == 1)
					{
						rowBits[row] |= bitField;
						columnBits[column] |= bitField;
						nonetBits[nonet] |= bitField;
						_BitScanForward((unsigned long*)&puzzle[j], (unsigned long)bitField);

						squaresSolved++;
					}
				}


			}
			unsolvedSquares -= squaresSolved;

		} while (unsolvedSquares && squaresSolved);

		if (!unsolvedSquares)
			break;

		//'Hidden Singles' deduction loop
		for (j = 0; j < arraylength; j++)
		{
			hiddenSingleFound = false;

			if (__builtin_popcount(possibleBits[j]) != 1)
			{
				rowOtherBits = 0;
				columnOtherBits = 0;
				nonetOtherBits = 0;
				jump = 0;
				bitField = possibleBits[j];

				GetPosition(j, &row, &column, &nonet);

				xBox = nonet / nonetwidth;
				yBox = nonet % nonetwidth;

				//In the loop we collect all the possible values at each square within the current..
				//..row/column/nonet EXCLUDING the possible values of the square under evaluation
				for (i = 0; i < sudokuwidth; i++)
				{
					rowIndex = row * sudokuwidth + i;
					columnIndex = i * sudokuwidth + column;
					nonetIndex = (xBox * nonetwidth) + (yBox * (sudokuwidth * nonetwidth)) + i + jump;

					if ((i % nonetwidth) == (nonetwidth - 1))
						jump += nonetwidth * (nonetwidth - 1);

					if (__builtin_popcount(possibleBits[rowIndex]) != 1 && rowIndex != j)
						rowOtherBits |= possibleBits[rowIndex];

					if (__builtin_popcount(possibleBits[columnIndex]) != 1 && columnIndex != j)
						columnOtherBits |= possibleBits[columnIndex];

					if (__builtin_popcount(possibleBits[nonetIndex]) != 1 && nonetIndex != j)
						nonetOtherBits |= possibleBits[nonetIndex];
				}

				//If a square has a possible value which NO OTHER square within the..
				//..current row, column or nonet possesses then the square MUST take that value
				rowResult = bitField & ~rowOtherBits;
				columnResult = bitField & ~columnOtherBits;
				nonetResult = bitField & ~nonetOtherBits;

				if (__builtin_popcount(rowResult) == 1)
				{
					possibleBits[j] = rowResult & bitMask;
					hiddenSingleFound = true;
				}
				else if (__builtin_popcount(columnResult) == 1)
				{
					possibleBits[j] = columnResult & bitMask;
					hiddenSingleFound = true;
				}
				else if (__builtin_popcount(nonetResult) == 1)
				{
					possibleBits[j] = nonetResult & bitMask;
					hiddenSingleFound = true;
				}

				if (hiddenSingleFound)
				{
					unsolvedSquares--;
					rowBits[row] |= possibleBits[j];
					columnBits[column] |= possibleBits[j];
					nonetBits[nonet] |= possibleBits[j];
					_BitScanForward((unsigned long*)&puzzle[j], (unsigned long)possibleBits[j]);
					//As we are updating the solved values some more solutions might drop out with the..
					//first solver, so break here and return to the 'naked single' solver
					break;

				}
			}

		}

	} while (unsolvedSquares && hiddenSingleFound);

	//The deductive solvers have done all they can, we now resort to backtracking to solve the remaining squares
	//however we only try the values which are legal in isolation for each square by bit scanning forward through possibleBits[]
	if (unsolvedSquares)
	{
		//Initialise arrays
		for (i = 0; i < arraylength; i++)
		{
			if (__builtin_popcount(possibleBits[i]) == 1)
				bitCurrentlyLegal[i] = possibleBits[i];
			else
			{
				bitCurrentlyLegal[i] = 0;
				_BitScanForward((unsigned long*)&j, (unsigned long)possibleBits[i]);
				firstPossibleBit[i] = 1 << j;
			}
		}

		i = 0;
		backTracking = false;

		while (i < arraylength)
		{
			if (__builtin_popcount(possibleBits[i]) != 1)
			{
				GetPosition(i, &row, &column, &nonet);

				if (bitCurrentlyLegal[i])
				{
					//We have backtracked to this square, we need to erase its value from..
					//..the thought solved arrays before we scan to the next possible bit
					rowBits[row] &= ~bitCurrentlyLegal[i];
					columnBits[column] &= ~bitCurrentlyLegal[i];
					nonetBits[nonet] &= ~bitCurrentlyLegal[i];
				}
				else
				{
					//The square is blank, fill it in with the first possible bit
					bitCurrentlyLegal[i] = firstPossibleBit[i];

					if (!(bitCurrentlyLegal[i] & rowBits[row]) &&
						!(bitCurrentlyLegal[i] & columnBits[column]) &&
						!(bitCurrentlyLegal[i] & nonetBits[nonet]))
					{
						//The first possible bit is legal we can continue to the next square
						rowBits[row] |= bitCurrentlyLegal[i];
						columnBits[column] |= bitCurrentlyLegal[i];
						nonetBits[nonet] |= bitCurrentlyLegal[i];
						backTracking = false;
						i++;
						continue;

					}
				}

				_BitScanForward((unsigned long*)&currentBitIndex, (unsigned long)bitCurrentlyLegal[i]);
				bitCurrentlyLegal[i] = 0;
				backTracking = true;

				//We now increment through the next possible bits until we find a legal one, otherwise backtrack
				//The loop can run while there are more significant bits to evaluate
				while (possibleBits[i] & rollingMask[currentBitIndex])
				{
					//We mask off the lower bits to scan forward to the next possible bit
					_BitScanForward((unsigned long*)&nextBitIndex, (unsigned long)(possibleBits[i] & rollingMask[currentBitIndex]));
					currentBitIndex = nextBitIndex;
					bitToTest = 1 << currentBitIndex;

					if (!(bitToTest & rowBits[row]) &&
						!(bitToTest & columnBits[column]) &&
						!(bitToTest & nonetBits[nonet]))
					{
						//We have found a legal value for this square for the time being
						//Update the arrays with the thought legal value
						bitCurrentlyLegal[i] = bitToTest;
						rowBits[row] |= bitToTest;
						columnBits[column] |= bitToTest;
						nonetBits[nonet] |= bitToTest;
						backTracking = false;
						break;
					}
				}
				//If we exited the loop above without breaking then there are currently no.. 
				//..legal values for the current square. We leave it blank and backtrack to..
				//..the previous sqaure
			}

			i += backTracking ? -1 : 1;
		}

		for (i = 0; i < arraylength; i++)
		{
			if (puzzle[i] == sudokuwidth)
			{
				_BitScanForward((unsigned long*)&puzzle[i], (unsigned long)bitCurrentlyLegal[i]);
				unsolvedSquares--;
			}
		}
	}

	return unsolvedSquares;
}