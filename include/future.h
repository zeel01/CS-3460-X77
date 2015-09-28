
#pragma once

#include "cs477.h"

namespace cs477
{

	namespace details
	{
		class basic_shared_state;
	}



	template <typename T>
	class future
	{
	public:
		future();
		~future();

		future(future &&f);
		future &operator =(future &&f);

		future(const future &&f) = delete;
		future &operator =(const future &f) = delete;

	public:
		void wait();
		T get();

	private:
		details::basic_shared_state *state;

		template <typename Fn> friend future<T> queue_work(Fn fn);
	};

	template <>
	class future<void> : public future<char>
	{
	public:
		future();
		~future();

		future(future &&f);
		future &operator =(future &&f);

		future(const future &&f) = delete;
		future &operator =(const future &f) = delete;

	public:
		void wait();
		void get();

	private:
		details::basic_shared_state *state;
	};


}