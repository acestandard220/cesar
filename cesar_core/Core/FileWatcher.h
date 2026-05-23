#pragma once
#include "../cesar_core.h"
#include "../Event/Delegate.h"


#include <filesystem>
#include <unordered_map>
#include <fstream>

namespace cesar
{
	DECLARE_EVENT(FileChangedEvent, FileWatcher, const std::string&);

	class FileWatcher {
	public:
		FileWatcher()
		{

		}
		~FileWatcher() = default;

		FileChangedEvent& GetFileChangedEvent() { return file_changed_event; }

		inline void Watch(const std::string& path, cesar::Bool recursive = true)
		{
			if (!std::filesystem::exists(path))
			{
				LOG_ERROR("Path specified to be watched does not exist.");
				return;
			}

			if (recursive)
			{
				for (auto& dir : std::filesystem::recursive_directory_iterator(path))
				{
					if (dir.is_regular_file()) {
						file_time_map[dir.path().string()] = std::filesystem::last_write_time(dir); 
					}
				}
			}
			else {
				for (auto& dir : std::filesystem::directory_iterator(path))
				{
					if (dir.is_regular_file()) {
						file_time_map[dir.path().string()] = std::filesystem::last_write_time(dir);
					}
				}
			}
		}

		void CheckWatchedFiles()
		{
			std::vector<std::pair<std::string, std::filesystem::file_time_type>> updates;

			for (const auto& [file, file_time] : file_time_map)
			{
				std::error_code ec;
				const auto last_write_time = std::filesystem::last_write_time(file, ec);

				if (ec) {
					LOG_WARN("Watched file inaccessible: {}", file);
					continue;
				}

				if (last_write_time != file_time) {
					updates.emplace_back(file, last_write_time);
				}
			}

			for (auto& [file, new_time] : updates) {
				file_time_map[file] = new_time;
				file_changed_event.Broadcast(file);
			}
		}

	private:
		std::unordered_map<std::string, std::filesystem::file_time_type> file_time_map;
		FileChangedEvent file_changed_event;
	};
}