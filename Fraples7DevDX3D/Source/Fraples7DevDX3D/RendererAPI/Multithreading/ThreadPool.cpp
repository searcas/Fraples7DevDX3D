#include "ThreadPool.h"


namespace FraplesDev
{
	//Multithreading
	namespace MT
	{
		ThreadPool::ThreadPool(size_t numOfThreads)
		{
			for (unsigned int i = 0u; i < numOfThreads; ++i)
			{
				_mThreadPool.emplace_back(std::thread(&ThreadPool::Execute, this));
			}
		}
		ThreadPool::~ThreadPool()
		{
			Stop();
			_mSignal.notify_all();
			for (auto& thread : _mThreadPool)
			{
				thread.join();
			}
		}
		void ThreadPool::Stop()
		{
			std::unique_lock<std::mutex>lock(_mMutex);
			_mStopThreads = true;
		}
		void ThreadPool::Execute()
		{
			while (true)
			{
				Task task;
				{
					std::unique_lock<std::mutex>lock(_mMutex);
					_mSignal.wait(lock, [=] {return _mStopThreads || !_mTaskQueue.queue.empty(); });
					if (_mStopThreads)
					{
						break;
					}
					task = std::move(_mTaskQueue.queue.front());
					_mTaskQueue.queue.pop();
				}
				task();
			}
		}
	}
}