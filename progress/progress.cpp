#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <string>

std::mutex mtx; 

void progressBar(int threadNum, int duration) {
	std::string bar = "[                    ]";
	int barWidth = 20;

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < barWidth; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(duration / barWidth));

		{
			std::lock_guard<std::mutex> lock(mtx);
			bar[i + 1] = '#';
			std::cout << "\rПоток " << threadNum << ": " << bar << std::flush;
		}
	}

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	{
		std::lock_guard<std::mutex> lock(mtx);
		std::cout << " ✅ Завершён за " << elapsed.count() << " секунд\n";
	}
}

int main() {
	setlocale(LC_ALL, "RUS");
	int numThreads = 5; 
	int duration = 3000; 

	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; i++) {
		threads.emplace_back(progressBar, i + 1, duration);
	}


	for (auto &th : threads) {
		th.join();
	}

	std::cout << "\nВсе потоки завершены!\n";
	return 0;
}