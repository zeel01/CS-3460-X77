
#pragma once

#include "cs477.h"



namespace cs477
{

	template <typename T> class future;
	template <typename T> class promise;

	template <typename T, typename Iterator> future<std::vector<T>> when_all(Iterator first, Iterator last);

	enum class future_state
	{
		not_ready = 0,
		has_value,
		has_error,
	};

	namespace details
	{

		class abstract_task;


		class basic_shared_state
		{
		public:
			basic_shared_state();
			virtual ~basic_shared_state();

			void set_exception(std::exception_ptr ptr);
			void wait();

		public:
			future_state state;

			std::exception_ptr ex;

			mutex mtx;
			condition_variable cv;

			std::unique_ptr<abstract_task> next;
		};

		template <typename T>
		class shared_state : public basic_shared_state
		{
		public:
			void set(T val);
			T get();

		public:
			T value;
		};

		template <>
		class shared_state<void> : public basic_shared_state
		{
		public:
			void set();
			void get();
		};


		class abstract_task
		{
		public:
			virtual ~abstract_task()
			{
			}

			virtual void execute() = 0;
		};

		template <typename R, typename Fn>
		class task : public abstract_task
		{
		public:
			task(Fn fn)
				: fn(std::move(fn))
			{
			}

			virtual void execute()
			{
				try
				{
					auto r = fn();
					p.set(std::move(r));
				}
				catch (...)
				{
					p.set_exception(std::current_exception());
				}
			}

			Fn fn;
			promise<R> p;
		};

		template <typename Fn>
		class task<void, Fn> : public abstract_task
		{
		public:
			task(Fn fn)
				: fn(std::move(fn))
			{
			}

			virtual void execute()
			{
				try
				{
					fn();
					p.set();
				}
				catch (...)
				{
					p.set_exception(std::current_exception());
				}
			}

			Fn fn;
			promise<void> p;
		};

		template <typename R, typename Fn> task<R, Fn> *make_task(Fn fn)
		{
			return new task<R, Fn>(std::move(fn));
		}

		template <typename R, typename Fn> void queue_work(task<R, Fn> *task)
		{
			auto work = CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID Context, PTP_WORK work)
			{
				auto task = (details::task<R, Fn> *)Context;
				task->execute();
				delete task;
				CloseThreadpoolWork(work);
			}, task, nullptr);
			SubmitThreadpoolWork(work);
		}


		template <typename T, typename Fn>
		auto then(std::shared_ptr<shared_state<T>> state, Fn fn)
		{
			using R = decltype(fn(future<T>{state}));
			future<R> f;

			lock_guard<> lock(state->mtx);

			auto task = make_task<R>([state, fn = std::move(fn)]() mutable
			{
				return fn(future<T>{ state });
			});
			f = task->p.get_future();

			if (state->state == future_state::not_ready)
			{
				state->next = std::unique_ptr<details::abstract_task>(task);
			}
			else
			{
				queue_work(task);
			}

			return f;
		}


	}


	template <typename Fn> future<typename std::result_of<Fn()>::type> queue_work(Fn fn);


	template <typename T>
	class future
	{
	public:
		future()
		{
		}

		virtual ~future()
		{
		}

		future(std::shared_ptr<details::shared_state<T>> state)
			: state(state)
		{
		}

		future(future &&f)
			: state(f.state)
		{
			f.state = nullptr;
		}

		future &operator =(future &&f)
		{
			std::swap(state, f.state);
			return *this;
		}

		future(const future &&f) = delete;
		future &operator =(const future &f) = delete;

	public:
		void wait()
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->wait();
		}

		T get()
		{
			if (!state)
			{
				// TODO: Throw
			}
			return state->get();
		}

		template<class Fn>
		auto then(Fn fn)
		{
			if (!state)
			{
				// TODO: Throw
			}

			return details::then(state, std::move(fn));
		}

	public:
		std::shared_ptr<details::shared_state<T>> state;

	};



	template <>
	class future<void>
	{
	public:
		future()
		{
		}

		~future()
		{
		}

		future(std::shared_ptr<details::shared_state<void>> state)
			: state(state)
		{
		}

		future(future &&f)
			: state(f.state)
		{
			f.state = nullptr;
		}

		future &operator =(future &&f)
		{
			std::swap(state, f.state);
			return *this;
		}

		future(const future &&f) = delete;
		future &operator =(const future &f) = delete;

	public:
		void wait()
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->wait();
		}

		void get()
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->wait();
		}

		template<class Fn>
		auto then(Fn fn)
		{
			if (!state)
			{
				// TODO: Throw
			}

			return details::then(state, std::move(fn));
		}

	private:
		std::shared_ptr<details::shared_state<void>> state;
	};


	template <typename T> future<T> make_ready_future(T value)
	{
		auto state = std::make_shared<details::shared_state<T>>();
		state->set(std::move(value));
		return state;
	}

	inline future<void> make_ready_future()
	{
		auto state = std::make_shared<details::shared_state<void>>();
		state->set();
		return state;
	}

	template <typename Fn> future<std::result_of_t<Fn()>> queue_work(Fn fn)
	{
		using R = typename std::result_of_t<Fn()>;
		auto task = new details::task<R, Fn>(std::move(fn));
		auto f = task->p.get_future();
		details::queue_work(task);
		return f;
	}





	template <typename T>
	class promise
	{
	public:
		promise()
			: state(std::make_shared<details::shared_state<T>>())
		{
		}

		virtual ~promise()
		{
		}

		promise(promise &&p)
			: state(p.state)
		{
			p.state = nullptr;
		}

		promise &operator =(promise &&p)
		{
			std::swap(state, p.state);
			return *this;
		}

		promise(const promise &p)
			: state(p.state)
		{
		}

		promise &operator =(const promise &p)
		{
			state = p.state;
			return *this;
		}

	public:
		void set(T value)
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->set(std::move(value));
		}

		void set_exception(std::exception_ptr err)
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->set_exception(err);
		}

		future<T> get_future()
		{
			return future<T>{state};
		}

	private:
		std::shared_ptr<details::shared_state<T>> state;
	};

	template <>
	class promise<void>
	{
	public:
		promise()
			: state(std::make_shared<details::shared_state<void>>())
		{
		}

		virtual ~promise()
		{
		}

		promise(promise &&p)
			: state(p.state)
		{
			p.state = nullptr;
		}

		promise &operator =(promise &&p)
		{
			std::swap(state, p.state);
			return *this;
		}

		promise(const promise &p)
			: state(p.state)
		{
		}

		promise &operator =(const promise &p)
		{
			state = p.state;
			return *this;
		}

	public:
		void set()
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->set();
		}

		void set_exception(std::exception_ptr err)
		{
			if (!state)
			{
				// TODO: Throw
			}
			state->set_exception(err);
		}

		future<void> get_future()
		{
			return future<void>{state};
		}

	private:
		std::shared_ptr<details::shared_state<void>> state;
	};





	template <typename T, typename Iterator> future<std::vector<T>> when_all(Iterator first, Iterator last)
	{
		promise<std::vector<T>> p;
		auto f = p.get_future();

		std::vector<T> list;
		std::exception_ptr ex;
		while (first != last)
		{
			try
			{
				list.push_back(first->get());
			}
			catch (...)
			{
				ex = std::current_exception();
			}

			first++;
		}

		if (ex)
		{
			p.set_exception(ex);
		}
		else
		{
			p.set(std::move(list));
		}

		return f;
	}



	//inline future<void> do_while(std::function<future<bool>()> body) 
	//{
	//	return queue_work([=]
	//	{
	//		while (body().get())
	//		{
	//		}
	//	});
	//}



	namespace details
	{

		template <class Body>
		void task_loop(promise<void> p, Body body)
		{
			body().then([=](auto f) mutable
			{
				try
				{
					if (f.get())
					{
						task_loop(p, body);
					}
					else
					{
						p.set();
					}
				}
				catch (...)
				{
					p.set_exception(std::current_exception());
				}
				return 0;
			});
		}
	}

	template <class Body>
	future<void> do_while(Body body)
	{
		promise<void> p;
		auto f = p.get_future();

		queue_work([=]() mutable
		{
			try
			{
				details::task_loop(p, body);
			}
			catch (...)
			{
				p.set_exception(std::current_exception());
			}
		});

		return f;
	}



}