#pragma once

#include<condition_variable>
#include<cstddef>
#include<deque>
#include<functional>
#include<mutex>
#include<thread>
#include<vector>

namespace keel {

class thread_pool {
public:
	explicit thread_pool(std::size_t worker_count = 2);

	~thread_pool();

	thread_pool(const thread_pool&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;

	void submit(std::function<void()> task);

	std::size_t pending() const;
	
	void shutdown();

private:
	void worker_loop(std::size_t id);

	std::vector<std::thread> workers_;
	mutable std::mutex mutex_;
	std::condition_variable cv_;
	std::deque<std::function<void()>> tasks_;
	bool stopping_ = false;

};


} //namespace keel