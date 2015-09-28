
#pragma once


namespace cs477
{
	template <typename T>
	inline future<T>::future()
		: state(nullptr)
	{
	}

	template <typename T>
	inline future<T>::~future()
	{
		if (state)
		{
			state->release();
		}
	}

	template <typename T>
	inline future<T>::future(future &&f)
		: state(f.state)
	{
		f.state = nullptr;
	}

	template <typename T>
	inline future<T> &future<T>::operator =(future &&f)
	{
		std::swap(state, f.state);
	}

	template <typename T>
	inline void future<T>::wait()
	{
		if (!state)
		{
			// TODO: Throw
		}
		state->wait();
	}

	template <typename T>
	inline T future<T>::get()
	{
		if (!state)
		{
			// TODO: Throw
		}
		return state->get();
	}

	template <typename Fn> future<typename std::result_of<Fn()>::type> queue_work(Fn fn)
	{
		using result_type = typename std::result_of<Fn()>::type;
		future<result_type> f;
		f.state = new details::shared_state<result_type, Fn>(std::move(fn));
		details::queue_work(f.state);
		return f;
	}

}