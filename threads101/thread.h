
#pragma once

#include <exception>



#ifdef _WIN32

using thread = HANDLE;

#else

using thread = pthread_t;

#endif




struct create_thread_param
{
	void(*function)(void *);
	void *context;
};



#ifdef _WIN32

inline DWORD __stdcall thread_start_routine(void *context)
{
	auto param = static_cast<create_thread_param *>(context);
	param->function(param->context);
	return 0;
}

#else

inline void *thread_start_routine(void *context)
{
	auto param = static_cast<create_thread_param *>(context);
	param->function(param->context);
	return nullptr;
}

#endif




inline thread create_thread(void(*function)(void *), void *context)
{
	create_thread_param param;
	param.function = function;
	param.context = context;

#ifdef _WIN32

	auto handle = CreateThread(nullptr, 0, thread_start_routine, &param, 0, nullptr);
	if (!handle)
	{
		auto error = GetLastError();
		throw std::system_error(error, std::generic_category());
	}

	return handle;

#else

	pthread_t tid;
	auto error = pthread_create(&tid, nullptr, thread_start_routine, &param);
	if (error)
	{
		throw std::system_error(error, std::generic_category());
	}
	return tid;

#endif

}



inline void join(thread thread)
{
#ifdef _WIN32

	auto result = WaitForSingleObject(thread, INFINITE);
	if (result != WAIT_OBJECT_0)
	{
		auto error = GetLastError();
		throw std::system_error(error, std::generic_category());
	}

	CloseHandle(thread);

#else 

	auto error = pthread_join(thread, nullptr);
	if (error)
	{
		throw std::system_error(error, std::generic_category());
	}

#endif
}



struct basic_thread_param
{
	virtual void operator()() const = 0;
};

template <typename Fn>
struct thread_param : public basic_thread_param
{
	thread_param(Fn fn)
		: fn(std::move(fn))
	{
	}

	virtual void operator()() const
	{
		fn();
	}

	Fn fn;
};


#ifdef _WIN32

inline DWORD __stdcall thread_start_routine2(void *context)
{
	auto param = static_cast<basic_thread_param *>(context);
	(*param)();
	delete param;
	return 0;
}

#else

inline void *thread_start_routine2(void *context)
{
	auto param = static_cast<basic_thread_param *>(context);
	(*param)();
	delete param;
	return nullptr;
}

#endif


template <typename Fn>
thread create_thread(Fn fn)
{
	auto param = new thread_param<Fn>(std::move(fn));

#ifdef _WIN32

	auto handle = CreateThread(nullptr, 0, thread_start_routine2, param, 0, nullptr);
	if (!handle)
	{
		auto error = GetLastError();
		throw std::system_error(error, std::generic_category());
	}

	return handle;

#else

	pthread_t tid;
	auto error = pthread_create(&tid, nullptr, thread_start_routine2, param);
	if (error)
	{
		throw std::system_error(error, std::generic_category());
	}
	return tid;

#endif

}