#include "SudokuSolver.h"
#include "array1465.h"
#include <chrono>
#include <iostream>

int main(int argc, char* argv[])
{
	size_t index = 0;

	//Report progress in seperate thread to not affect solve time
	auto reportFunction = [&index](const size_t size) {
		size_t count = 0;
		while (index != size)
		{
			if (count != index)
			{
				std::cout << index << " solved" << std::endl;
				count = index;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}

		std::cout << "All " << index << " solved!" << std::endl;

		return;
	};

	std::thread reportingThread(reportFunction, top1465.size());


	//Measure time to solve all sudokus
	std::chrono::time_point startTime = std::chrono::steady_clock::now();

	for (auto& puzzle : top1465)
	{
		SudokuSolver(puzzle);	
		index++;
	}

	std::chrono::duration<float> totalTime =  std::chrono::steady_clock::now() - startTime;

	if (reportingThread.joinable())
		reportingThread.join();

	std::cout << "Total solve time: " << totalTime << std::endl;

	return 0;
}