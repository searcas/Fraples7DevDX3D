#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <future>
#include <condition_variable>
#include <queue>
namespace FraplesDev
{
	//Multithreading
	namespace MT
	{
		using Task = std::function<void()>;
		struct TaskQueue
		{
			std::mutex mutex;
			std::queue<Task> queue;
		};
		class ThreadPool
		{
		public:
			ThreadPool(size_t numOfThreads);
			~ThreadPool();
			void Stop();
			template<class T>
			auto AddTask(T task) -> std::future<decltype(task())>
			{
				using task_return_t = decltype(task());
				auto pTask = std::make_shared< std::packaged_task<task_return_t()>>(std::move(task));
				{
					std::unique_lock<std::mutex>lock(_mMutex);
					_mTaskQueue.queue.emplace([=] 
					{
						(*pTask)();   // Add a lambda function to the task queue which
					});			     // calls the package_task<>'s callable object -> T task 
				}
				_mSignal.notify_one();
				return pTask->get_future();
			}
		private:
			void Execute();
		private:
			std::vector<std::thread>_mThreadPool;
			std::mutex _mMutex;
			std::condition_variable _mSignal;
			bool _mStopThreads = false;
			TaskQueue _mTaskQueue;
		};

	}
}
