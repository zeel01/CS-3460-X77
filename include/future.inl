
#pragma once


namespace cs477
{



	template <typename Fn> future<typename std::result_of<Fn()>::type> queue_work(Fn fn)
	{
		using result_type = typename std::result_of<Fn()>::type;
		future<result_type> f;
		auto state = new details::shared_state<result_type, Fn>(std::move(fn));
		f.state = state;
		details::queue_work(f.state);
		return f;
	}

}