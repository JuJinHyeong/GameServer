#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() {};
	~ThreadSafeQueue() {};

	void Push(T data) {
		std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(data);
		mCv.notify_one();
	}
	T Pop() {
		std::unique_lock<std::mutex> lock(mMutex);
		mCv.wait(lock, [this] { return !mQueue.empty(); });
		T value = mQueue.front();
		mQueue.pop();
		return value;
	}
	bool Empty() {
		return mQueue.empty();
	}
private:
	std::queue<T> mQueue;
	std::mutex mMutex;
	std::condition_variable mCv;
};