
#pragma once

#include "cs477.h"

namespace cs477
{

	namespace details
	{
		class basic_shared_state;
	}



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

	private:
		details::basic_shared_state *state;

		template <typename Fn> friend future queue_work(Fn fn);
	};



	template <typename Iter>
	future when_all(Iter first, Iter last)
	{
		
		for
	}
	

}