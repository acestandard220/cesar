#include "TextureLoadManager.h"

namespace cesar
{
	void TextureJobManager::Wait()
	{
		context->Wait();
	}

	void TextureJobManager::Submit(const Job& job)
	{
		std::lock_guard lock(generic_mutex);
		generic_jobs.emplace_back(job);
	}


	/// <summary>
	/// This function calls ExecuteAll() on the job queue, Submits the new job & Immediately executes 
	/// This function guarantees GPU completion. It forces a Wait()
	/// </summary>
	/// <param name="job"></param>
	void TextureJobManager::ExecuteImmediate(const Job& job)
	{
		ExecuteAll();
		Submit(job);
		ExecuteAll();
		Wait();
	}

	void TextureJobManager::ExecuteAll()
	{
		std::vector<std::function<void()>> jobs;

		{
			std::lock_guard lock(generic_mutex);
			jobs.swap(generic_jobs);
		}

		std::lock_guard lock(cmd_list_mutex);
		context->Begin();

		for (const auto& job : jobs)
		{
			job();
		}

		context->End();
	}
}
