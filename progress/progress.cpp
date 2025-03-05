#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <windows.h>
#include <string>

std::mutex mtx; 
const int barWidth = 20;
std::vector<std::string> bars;

void setCursorPosition(int y) {
	COORD coord;
	coord.X = 0;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void progressBar(int threadNum, int duration) {
	std::string bar = "[                    ]"; 
	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < barWidth; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(duration / barWidth));

		{
			std::lock_guard<std::mutex> lock(mtx);
			bar[i + 1] = '#';
			bars[threadNum] = "Поток " + std::to_string(threadNum + 1) + ": " + bar;

			setCursorPosition(threadNum);
			std::cout << bars[threadNum] << std::flush;
		}
	}

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	{
		std::lock_guard<std::mutex> lock(mtx);
		bars[threadNum] += " ✅ " + std::to_string(elapsed.count()) + " сек";
		setCursorPosition(threadNum);
		std::cout << bars[threadNum] << std::flush;
	}
}

int main() {
	setlocale(LC_ALL, "RUS");
	int numThreads = 5;
	int duration = 3000;

	bars.resize(numThreads, "Поток X: [                    ]");

	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; i++) {
		threads.emplace_back(progressBar, i, duration);
	}

	for (auto &th : threads) {
		th.join();
	}

	{
		std::lock_guard<std::mutex> lock(mtx);
		setCursorPosition(numThreads);
		std::cout << "\nВсе потоки завершены!\n";
	}

	return 0;
}