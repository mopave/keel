#include<chrono>
#include<cstdio>
#include<thread>

#include"core/thread_pool.h"

int main() {
	{
		keel::thread_pool pool(2);

		for (int i = 1; i <= 20; ++i) {
			pool.submit([i] {
				std::printf("task %2d start\n", i);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				std::printf("task %2d done\n", i);
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		std::printf("pending when leaving scope: %zu\n", pool.pending());
	}

	std::printf("all done\n");
	return 0;
}