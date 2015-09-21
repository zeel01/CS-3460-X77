
#pragma once

#include <atomic>



namespace cs477
{

#ifdef _WIN32
	using thread = HANDLE;
#else
	using thread = pthread_t;
#endif


	namespace details
	{

		// Base class for thread routine parameters.
		struct basic_thread_param
		{
			virtual ~basic_thread_param() { };

			// Derived classes must implement this method.
			virtual void operator()() const = 0;

			virtual void execute() const
			{
				try
				{
					// Call the derived class.
					(*this)();
				}
				catch (...)
				{
					// Just ignore any excpetions.
				}

				delete this;
			}
		};

		struct simple_thread_param : public basic_thread_param
		{
			simple_thread_param(void(*function)(void *), void *context)
				: function(function), context(context)
			{
			}

			virtual void operator()() const
			{
				if (function)
				{
					function(context);
				}
			}

			void(*function)(void *);
			void *context;
		};

		template <typename Fn>
		struct thread_param : public basic_thread_param
		{
			thread_param(Fn function)
				:function(std::move(function))
			{
			}

			virtual void operator()() const
			{
				function();
			}

			Fn function;
		};






#ifdef _WIN32

		inline DWORD __stdcall thread_start_routine(void *context)
		{
			auto param = static_cast<basic_thread_param *>(context);
			param->execute();
			return 0;
		}

#else

		inline void *thread_start_routine(void *context)
		{
			auto param = static_cast<basic_thread_param *>(context);
			param->execute();
			return nullptr;
		}

#endif

		inline thread create_thread(basic_thread_param *param)
		{
#ifdef _WIN32

			auto handle = CreateThread(nullptr, 0, thread_start_routine, param, 0, nullptr);
			if (!handle)
			{
				auto error = GetLastError();
				throw std::system_error(error, std::generic_category());
			}

			return handle;

#else

			pthread_t tid;
			auto error = pthread_create(&tid, nullptr, thread_start_routine, param);
			if (error)
			{
				throw std::system_error(error, std::generic_category());
			}
			return tid;

#endif

		}




		class basic_shared_state
		{
		public:
			std::atomic<int> ref;

			enum
			{
				ready = 0,
				is_signaled,
				is_error,
			} state;

			std::exception_ptr ex;
			mutex mtx;
			condition_variable cv;


			basic_shared_state()
				: ref(1), state(ready)
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

			void signal()
			{
				lock_guard<> guard(mtx);
				if (state != ready)
				{
					// TODO: Throw
				}
				state = is_signaled;
				cv.notify_all();
			}

			void set_exception(std::exception_ptr ptr)
			{
				lock_guard<> guard(mtx);
				if (state != ready)
				{
					// TODO: Throw
				}

				state = is_error;
				ex = std::move(ptr);
				cv.notify_all();
			}

			void wait()
			{
				lock_guard<> guard(mtx);
				while (state == ready)
				{
					cv.wait(mtx);
				}

				if (ex)
				{
					auto ptr = std::move(ex);
					std::rethrow_exception(ptr);
				}
			}

			void execute()
			{
				try
				{
					(*this)();
					signal();
				}
				catch (...)
				{
					set_exception(std::current_exception());
				}
			}

			virtual void operator()() const = 0;
		};


		template <typename Fn>
		class shared_state : public basic_shared_state
		{
		public:
			shared_state(Fn fn)
				: fn(std::move(fn))
			{
			}

			virtual void operator()() const
			{
				fn();
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

}