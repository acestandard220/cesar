#include "TextureLoadManager.h"

namespace cesar
{
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
