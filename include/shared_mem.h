
#pragma once

#include <unordered_map>


namespace cs477
{

	namespace details
	{
		struct shm_handle
		{
			~shm_handle()
			{
				if (buf) UnmapViewOfFile(buf);
				if (fh) CloseHandle(fh);
			}

			HANDLE fh;
			void *buf;
			size_t len;
		};

		struct shm_manager
		{
			~shm_manager()
			{
				handles.clear();
			}

			std::shared_ptr<shm_handle> open(const std::string &name, size_t size)
			{
				lock_guard<> lock(mtx);

				auto pos = handles.find(name);
				if (pos != handles.end())
				{
					assert(pos->second->len == size);
					return pos->second;
				}
				else
				{
					auto mem = std::make_shared<shm_handle>();

					mem->fh = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, static_cast<DWORD>(size), name.c_str());
					if (!mem->fh)
					{
						throw std::system_error(GetLastError(), std::system_category());
					}

					mem->len = size;
					mem->buf = MapViewOfFile(mem->fh, FILE_MAP_ALL_ACCESS, 0, 0, static_cast<DWORD>(size));
					if (!mem->buf)
					{
						throw std::system_error(GetLastError(), std::system_category());
					}

					handles[name] = mem;

					return mem;
				}
			}

			void close(void *ptr)
			{
				lock_guard<> lock(mtx);

				for (auto i = handles.begin(); i != handles.end(); i++)
				{
					auto &kvp = *i;
					if (kvp.second->buf == ptr)
					{
						handles.erase(i);
						return;
					}
				}
			}

			mutex mtx;
			std::unordered_map<std::string, std::shared_ptr<shm_handle>> handles;
		};

		static shm_manager &get_shm_manager()
		{
			static shm_manager manager;
			return manager;
		}

	}




	inline void *shm_alloc(const std::string &name, size_t size)
	{
		auto &manager = details::get_shm_manager();
		auto mem = manager.open(name, size);
		return mem->buf;
	}

	inline void shm_free(void *ptr)
	{
		auto &manager = details::get_shm_manager();
		manager.close(ptr);
	}


}