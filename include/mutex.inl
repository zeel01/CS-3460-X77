
#pragma once


namespace cs477
{




	inline mutex::mutex()
	{
#ifdef _WIN32
		InitializeCriticalSection(&cs);
#else
		pthread_mutex_init(&mtx, nullptr);
#endif
	}

	inline mutex::~mutex()
	{
#ifdef _WIN32
		DeleteCriticalSection(&cs);
#else
		pthread_mutex_destroy(&mtx);
#endif
	}

	inline void mutex::lock()
	{
#ifdef _WIN32
		EnterCriticalSection(&cs);
#else
		pthread_mutex_lock(&mtx);
#endif
	}

	inline void mutex::unlock()
	{
#ifdef _WIN32
		LeaveCriticalSection(&cs);
#else
		pthread_mutex_unlock(&mtx);
#endif
	}




	template <typename Lock>
	lock_guard<Lock>::lock_guard(Lock &lock)
		: my_lock(lock)
	{
		my_lock.lock();
	}

	template <typename Lock>
	lock_guard<Lock>::~lock_guard()
	{
		my_lock.unlock();
	}











	inline condition_variable::condition_variable()
	{
#ifdef _WIN32
		InitializeConditionVariable(&cv);
#else
		pthread_cond_init(&cv, nullptr);
#endif
	}

	inline condition_variable::~condition_variable()
	{
#ifdef _WIN32
#else
		pthread_cond_destroy(&cv);
#endif
	}

	inline void condition_variable::notify_one()
	{
#ifdef _WIN32
		WakeConditionVariable(&cv);
#else
		pthread_cond_signal(&mtx);
#endif
	}

	inline void condition_variable::notify_all()
	{
#ifdef _WIN32
		WakeAllConditionVariable(&cv);
#else
		pthread_cond_broadcast(&mtx);
#endif
	}

	inline void condition_variable::wait(mutex &mtx)
	{
		// Mutex must not be locked here!
#ifdef _WIN32
		if (!SleepConditionVariableCS(&cv, &mtx.cs, INFINITE))
		{
			throw std::exception();
		}
#else
		auto err = pthread_cond_wait(&cv, mtx.mtx);
		if (err)
		{
			throw std::exception();
		}
#endif
	}

#include <time.h>
	inline bool condition_variable::wait(mutex &mtx, std::chrono::milliseconds ms)
	{

#ifdef _WIN32
		if (!SleepConditionVariableCS(&cv, &mtx.cs, static_cast<DWORD>(ms.count())))
		{
			auto err = GetLastError();
			if (err == ERROR_TIMEOUT)
			{
				return false;
			}
			else
			{
				throw std::exception();
			}
		}
		return true;
#else
		timespec time;
		auto sec = ms.count() / 1000;
		time.tv_sec = static_cast<time_t>(sec);
		time.tv_nsec = (ms.count() - (sec * 1000)) * 10000;

		auto err = pthread_cond_timedwait(&cv, mtx.mtx, &time);
		if (!err)
		{
			return true;
		}
		else if (err == ETIMEDOUT)
		{
			return false;
		}
		else
		{
			throw std::exception();
		}
#endif
	}









}