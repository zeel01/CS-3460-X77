
#pragma once


namespace cs477
{



	template <typename Fn> future<typename std::result_of<Fn()>::type> queue_work(Fn fn)
	{
		using result_type = typename std::result_of<Fn()>::type;
		auto state = new details::shared_state<result_type, Fn>(std::move(fn));
		auto f = details::make_future(state);
		details::queue_work(state);
		return f;
	}

}