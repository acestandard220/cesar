#pragma once
#include "Resources/ImageTexture.h"

namespace cesar
{
	/// <summary>
	/// The functions of this class only handle future GPU work that does not require caller to worry about return values.
	/// It is set up such that only the GPU data is modified and no CPU side references have to be changed.
	/// It is for this reason textures and SRV indexes are created externally
	/// </summary>
	class TextureLoadManager
	{
	public:
		TextureLoadManager(OfflineContext* context)
			:context(context)
		{

		}

		~TextureLoadManager()
		{
			if (load_jobs.size()) {
				LOG_WARN("There are still textures being loaded");
			}
		}

		struct TextureLoadJob
		{
			ImageTexture* image_texture;
			TextureDesc desc;
			Subresource* subresources;
			Uint32 subresource_count;
			Buffer* data; 
			Uint64 size;
			Bool is_cooked;
		};

		struct MipGenerationJob
		{
			ImageTexture* image_texture;
			TextureDesc desc;
		};


		void SubmitLoadJob(const TextureLoadJob& load_job)
		{
			std::lock_guard lock(load_job_mutex);
			load_jobs.emplace_back(load_job);
		}

		void SubmitMipGenJob(const MipGenerationJob& mip_job)
		{
			std::lock_guard lock(gen_mip_job_mutex);
			mip_gen_jobs.emplace_back(mip_job);
		}

		void ExecuteTextureCreateJobs()
		{
			std::vector<TextureLoadJob> jobs;

			{
				std::lock_guard lock(load_job_mutex);
				jobs.swap(load_jobs);
			}

			context->Begin();

			for (const auto& job : jobs)
			{
				ImageTexture* image_texture = job.image_texture;
				const auto& gpu_texture = image_texture->gpu_texture.get();
				if (job.is_cooked)
				{
					context->UploadTextureData(job.data, job.size, job.subresources,job.subresource_count, gpu_texture);
				}
				else {
					context->UploadTextureData(job.data, gpu_texture);
				}
			}
			LOG_TRACE("Called by thread. {}", 1);

			context->End();
			
			for (const auto& job : jobs)
			{
				delete job.data;
			}
			jobs.clear();
		}

		void ExecuteMipGenerationJobs()
		{
			std::vector<MipGenerationJob> jobs;

			{
				std::lock_guard lock(gen_mip_job_mutex);
				jobs.swap(mip_gen_jobs);
			}

			context->Begin();

			for (const auto& job : jobs)
			{
				context->GenerateMips(job.image_texture->gpu_texture.get(), job.image_texture->srv_index);
			}

			context->End();

			jobs.clear();
		}

		void ExecuteAllJobs()
		{
			ExecuteTextureCreateJobs();
			ExecuteMipGenerationJobs();
		}


		inline Bool IsLoadBusy()const
		{
			return load_jobs.size();
		}

		inline Bool IsGenMipsBusy()const
		{
			return mip_gen_jobs.size();
		}

		Bool Busy()const
		{
			return IsLoadBusy() || IsGenMipsBusy();
		}

	private:
		std::mutex load_job_mutex;
		std::mutex gen_mip_job_mutex;

		std::vector<TextureLoadJob> load_jobs;
		std::vector<MipGenerationJob> mip_gen_jobs;
		OfflineContext* context;
	};
}