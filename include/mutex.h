
#pragma once


class mutex
{
public:
	mutex();

	mutex(const mutex &) = delete;
	mutex &operator =(const mutex &) = delete;

	mutex(mutex &&) = delete;
	mutex &operator =(mutex &&) = delete;

	~mutex();

public:
	void lock();
	void unlock();

private:
#ifdef _WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t mtx;
#endif

};




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
