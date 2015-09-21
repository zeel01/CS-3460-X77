
#pragma once


namespace cs477
{

	inline future::future()
		: state(nullptr)
	{
	}

	inline future::~future()
	{
		if (state)
		{
			state->release();
		}
	}

	inline future::future(future &&f)
		: state(f.state)
	{
		f.state = nullptr;
	}

	inline future &future::operator =(future &&f)
	{
		std::swap(state, f.state);
	}

	inline void future::wait()
	{
		if (!state)
		{
			// TODO: Throw
		}
		state->wait();
	}


	template <typename Fn> future queue_work(Fn fn)
	{
		future f;
		f.state = new details::shared_state<Fn>(std::move(fn));
		details::queue_work(f.state);
		return f;
	}

}