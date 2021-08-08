#ifndef netQueue_hpp
#define netQueue_hpp

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class TsQueue {
	std::queue<T> q;
	std::mutex queueMutex;

	std::condition_variable waitOn;
	std::mutex waitMutex;
public:
	void push(T elem) {
		std::unique_lock lock(queueMutex);

		q.push(elem);

		lock.unlock();
		waitOn.notify_one();
	}

	T pop() {
		std::scoped_lock lock(queueMutex);

		T elem = std::move(q.front());
		q.pop();
		return elem;
	}

	bool empty() {
		std::scoped_lock lock(queueMutex);

		return q.empty();
	}

	void wait() {
		while (empty()) {
			std::unique_lock lock(waitMutex);
			waitOn.wait(lock);
		// waitOn.wait(lock, [this]{!q.empty();});
		}
		// lock.unlock();
	}

	T& front() {
		std::scoped_lock lock(queueMutex);

		return q.front();
	}
};

#endif