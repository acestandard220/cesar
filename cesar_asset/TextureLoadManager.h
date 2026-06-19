#pragma once
#include "Resources/ImageTexture.h"

namespace cesar
{
	/// <summary>
	/// The functions of this class only handle future GPU work that does not require caller to worry about return values.
	/// It is set up such that only the GPU data is modified and no CPU side references have to be changed.
	/// It is for this reason textures and SRV indexes are created externally
	/// </summary>
	class TextureJobManager
	{
		using Job = std::function<void()>;
	public:
		TextureJobManager(OfflineContext* context)
			:context(context)
		{

		}

		~TextureJobManager()
		{
			
		}

		void Wait();

		void Submit(const Job& job);

		/// <summary>
		/// This function calls ExecuteAll() on the job queue, Submits the new job & Immediately executes 
		/// This function guarantees GPU completion. It forces a Wait()
		/// </summary>
		/// <param name="job"></param>
		void ExecuteImmediate(const Job& job);

		void ExecuteAll();

	private:
		std::mutex cmd_list_mutex;
		std::mutex generic_mutex;

		std::vector<Job> generic_jobs;

		OfflineContext* context;
	};
}