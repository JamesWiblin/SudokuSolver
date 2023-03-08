# SudokuSolver
Fast deductive and backtracking bitwise sudoku solver.

This is a two stage sudoku solver. First stage uses deductive methods 'naked singles' and 'hidden singles' to efficiently solve easy squares and reduce the number of possible values. Second stage is a classic backtracking method but uses the reduced possible value set computed in the first stage to decrease the number of iterations.

Both stages store the possible values for each square, row, column and nonet in a bit field. This allows us to test and eliminate values using simple and efficient bitwise operations.

For a good explanation on the naked and hidden singles techniques see:
http://www.sudocue.net/guide.php#NakedSingle
http://www.sudocue.net/guide.php#HiddenSingle

## TODO:
* Make code compiler agnostic. Currently only supports MSVC.
* Implement more advanced deductive methods.
