
#pragma once

#include "cs477.h"



namespace cs477
{

	template <typename T> class future;




	namespace details
	{

		class basic_shared_state
		{
		public:
			std::atomic<int> ref;

			enum
			{
				not_ready = 0,
				has_value,
				has_error,
			} state;

			std::exception_ptr ex;

			mutex mtx;
			condition_variable cv;


			basic_shared_state()
				: ref(1), state(not_ready)
			{
			}

			virtual ~basic_shared_state()
			{
			}

			void addref()
			{
				++ref;
			}

			void release()
			{
				if (ref == 1)
				{
					delete this;
					return;
				}
				--ref;
			}

			void set_exception(std::exception_ptr ptr)
			{
				lock_guard<> guard(mtx);
				if (state != not_ready)
				{
					// TODO: Throw
				}

				state = has_error;
				ex = std::move(ptr);
				cv.notify_all();
			}

			void set()
			{
				lock_guard<> guard(mtx);
				if (state != not_ready)
				{
					// TODO: Throw
				}
				state = has_value;
				cv.notify_all();
			}

			void wait()
			{
				lock_guard<> guard(mtx);
				while (state == not_ready)
				{
					cv.wait(mtx);
				}

				if (ex)
				{
					auto ptr = std::move(ex);
					std::rethrow_exception(ptr);
				}
			}

			virtual void execute() = 0;
		};

		template <typename T>
		class basic_shared_state_with_value : public basic_shared_state
		{
		public:
			T value;

		public:
			basic_shared_state_with_value()
			{
			}

			virtual ~basic_shared_state_with_value()
			{
			}

			void set(T &&val)
			{
				lock_guard<> guard(mtx);
				if (state != not_ready)
				{
					// TODO: Throw
				}
				state = has_value;
				value = std::move(val);
				cv.notify_all();
			}

			T get()
			{
				lock_guard<> guard(mtx);
				while (state == not_ready)
				{
					cv.wait(mtx);
				}

				if (ex)
				{
					auto ptr = std::move(ex);
					std::rethrow_exception(ptr);
				}

				return std::move(value);
			}


		};



		template <typename T, typename Fn>
		class shared_state : public basic_shared_state_with_value<T>
		{
		public:
			shared_state(Fn fn)
				: fn(std::move(fn))
			{
			}

			virtual void execute()
			{
				try
				{
					auto val = fn();
					set(std::move(val));
				}
				catch (...)
				{
					set_exception(std::current_exception());
				}
			}

			Fn fn;
		};

		template <typename Fn>
		class shared_state<void, Fn> : public basic_shared_state
		{
		public:
			shared_state(Fn fn)
				: fn(std::move(fn))
			{
			}

			virtual void execute()
			{
				try
				{
					fn();
					set();
				}
				catch (...)
				{
					set_exception(std::current_exception());
				}
			}

			Fn fn;
		};




		void queue_work(basic_shared_state *state)
		{
			state->addref();

#ifdef _WIN32
			auto work = CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE, PVOID context, PTP_WORK work)
			{
				auto state = static_cast<basic_shared_state *>(context);
				state->execute();
				state->release();
			}, state, nullptr);
			SubmitThreadpoolWork(work);

#else
			static_assert(false, "Get a threadpool");
#endif
		}




	}




	template <typename T>
	class future
	{
	public:
		future()
			: state(nullptr)
		{
		}

		virtual ~future()
		{
			if (state)
			{
				state->release();
			}
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

	private:
		details::basic_shared_state_with_value<T> *state;

		template <typename Fn> friend future<typename std::result_of<Fn()>::type> queue_work(Fn fn);
	};



	template <>
	class future<void>
	{
	public:
		future()
			: state(nullptr)
		{
		}

		~future()
		{
			if (state)
			{
				state->release();
			}
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


	private:
		details::basic_shared_state *state;
		template <typename Fn> friend future<typename std::result_of<Fn()>::type> queue_work(Fn fn);
	};


}