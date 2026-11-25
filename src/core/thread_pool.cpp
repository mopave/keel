#include"core/thread_pool.h"

#include<cstdio>
#include<exception>
#include<stdexcept>
#include<utility>

namespace keel {

thread_pool::thread_pool(std::size_t worker_count) {
	if (worker_count == 0) {
		throw std::invalid_argument("thread_pool : worker_count must be > 0");
	}
	workers_.reserve(worker_count);

	for (std::size_t i = 0; i < worker_count; ++i) {
		workers_.emplace_back(&thread_pool::worker_loop, this, i);
	}
}

thread_pool::~thread_pool() {
	shutdown();
}

void thread_pool::submit(std::function<void()> task) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (stopping_) {
			throw std::runtime_error("thread_pool: submit after shutdown");
		}
		tasks_.push_back(std::move(task));
	}
	cv_.notify_one();
}

std::size_t thread_pool::pending() const {
	std::lock_guard<std::mutex> lok(mutex_);
	return tasks_.size();
}

void thread_pool::shutdown() {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (stopping_) return;
		stopping_ = true;
	}
	cv_.notify_all();

	for (auto& t : workers_) {
		if (t.joinable()) t.join();
	}
	workers_.clear();
}

void thread_pool::worker_loop(std::size_t id) {
	for (;;) {
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] {return stopping_ || !tasks_.empty(); });

			if (stopping_ && tasks_.empty()) {
				return;
			}
			task = std::move(tasks_.front());
			tasks_.pop_front();
		}

		std::printf(" worker %zu took a task\n", id);

		try {
			task();
		}
		catch (const std::exception& e) {
			std::fprintf(stderr, " worker %zu: task threw: %s\n", id, e.what());
		}
		catch (...) {
			std::fprintf(stderr, " worker %zu: task threw unknown exception\n", id);
		}
	}
}

} //namespace keel