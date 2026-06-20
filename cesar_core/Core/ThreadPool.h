#pragma once
#include "../cesar_core.h"

//Reference: https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

namespace cesar
{

	class ThreadPool
	{
	public:
		ThreadPool()
		{
			const Uint32 max_thread_count = std::thread::hardware_concurrency();
			for (Uint32 i = 0; i < max_thread_count; i++)
			{
				threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
			}
		}

		template <class F>
		auto SubmitJob(F&& f) -> std::future<std::invoke_result_t<F>>
		{
			using R = std::invoke_result_t<F>;

			auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
			std::future<R> fut = task->get_future();

			{
				std::unique_lock lock(queue_mutex);
				jobs.emplace_back([task]() { (*task)(); });
			}

			mutex_condition.notify_one();
			return fut;
		}

		void SubmitJob(std::function<void()>& new_job)
		{
			{
				std::unique_lock lock(queue_mutex); //Just making sure main thread is the only one accessing the jobs pool at this moment
				jobs.push_back(new_job);
			}
			mutex_condition.notify_one();
		}

		void Shutdown()
		{
			{
				std::unique_lock lock(queue_mutex);
				done = true;
			}
			mutex_condition.notify_all();
			for (Uint32 i = 0; i < threads.size(); i++)
			{
				threads[i].join();
			}
			threads.clear();
		}

		static ThreadPool& GetThreadPool()
		{
			static ThreadPool pool;
			return pool;
		}


	private:
		void ThreadLoop()
		{
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock lock(queue_mutex);
					mutex_condition.wait(lock, [this]() {
						return !jobs.empty() || done;
						});

					if (done)
						return;
					job = jobs.back();
					jobs.pop_back();
				}
				job();
			}
		}
	private:
		std::vector<std::function<void()>> jobs;
		std::vector<std::thread> threads;
		std::mutex queue_mutex;
		std::condition_variable mutex_condition;
		Bool done = false;

	};

#define gThreadPool ThreadPool::GetThreadPool()

}