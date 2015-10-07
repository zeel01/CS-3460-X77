
#pragma once

#include "cs477.h"

namespace cs477
{

	namespace details
	{
		class socket;

		class async_io
		{
		public:
			enum io_type
			{
				recv,
				send,
			};
		public:
			async_io(io_type type)
				: type(type)
			{
				memset(&ol, 0, sizeof(OVERLAPPED));
			}
	
		public:
			OVERLAPPED ol;
			io_type type;
			std::string buf;
		};

		class async_send : public async_io
		{
		public:
			async_send()
				: async_io(async_io::send)
			{
			}

			promise<void> promise;
		};

		class async_recv : public async_io
		{
		public:
			async_recv(size_t count)
				: async_io(async_io::recv)
			{
				buf.resize(count);
			}

			promise<std::string> promise;
		};

		class socket
		{
		public:
			socket()
				: ref(1), handle(INVALID_SOCKET), io(nullptr)
			{
			}

			~socket()
			{
				if (handle != INVALID_SOCKET)
				{
					closesocket(handle);
				}
				if (io)
				{
					CloseThreadpoolIo(io);
				}
			}

		public:
			void addref()
			{
				++ref;
			}

			void release()
			{
				if (ref == 1)
				{
					delete this;
				}
				--ref;
			}

		public:
			void associate_with_threadpool()
			{
				io = CreateThreadpoolIo(reinterpret_cast<HANDLE>(handle), io_callback, nullptr, nullptr);
				if (!io)
				{
					throw std::exception();
				}
			}

			future<void> send(const char *buf, size_t len)
			{
				auto op = new async_send();
				op->buf = { buf, len };
				auto f = op->promise.get_future();

				StartThreadpoolIo(io);

				WSABUF wsabuf;
				wsabuf.buf = const_cast<char *>(buf);
				wsabuf.len = len;
				auto result = WSASend(handle, &wsabuf, 1, nullptr, 0, &op->ol, nullptr);
				if (result == SOCKET_ERROR)
				{
					if (GetLastError() != ERROR_IO_PENDING)
					{
						CancelThreadpoolIo(io);
						delete op;
						throw std::exception();
					}
				}

				return f;
			}

			future<std::string> recv(size_t len)
			{
				auto op = new async_recv(len);
				auto f = op->promise.get_future();

				StartThreadpoolIo(io);

				WSABUF wsabuf;
				wsabuf.buf = const_cast<char *>(op->buf.c_str());
				wsabuf.len = len;
				DWORD flags = 0;
				auto result = WSARecv(handle, &wsabuf, 1, nullptr, &flags, &op->ol, nullptr);
				if (result == SOCKET_ERROR)
				{
					if (GetLastError() != ERROR_IO_PENDING)
					{
						CancelThreadpoolIo(io);
						delete op;
						throw std::exception();
					}
				}

				return f;
			}

		private:
			static void __stdcall io_callback(PTP_CALLBACK_INSTANCE, PVOID, PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO)
			{
				auto io = (async_io *)Overlapped;
				if (io->type == async_io::recv)
				{
					auto recv = static_cast<async_recv *>(io);
					if (IoResult || !NumberOfBytesTransferred)
					{
						recv->promise.set_exception(std::make_exception_ptr(std::exception{}));
					}
					recv->buf.resize(NumberOfBytesTransferred);
					recv->promise.set(std::move(recv->buf));
					delete recv;
				}
				else if (io->type == async_io::send)
				{
					auto send = static_cast<async_send *>(io);
					if (IoResult || !NumberOfBytesTransferred)
					{
						send->promise.set_exception(std::make_exception_ptr(std::exception{}));
					}
					send->promise.set();
					delete send;
				}
				else
				{
					// ?
				}
			}

		public:
			std::atomic<int> ref;

			SOCKET handle;
			PTP_IO io;
		};

	}


	namespace net
	{
		class socket;
		class acceptor;

		void initialize();
		void finalize();

		sockaddr_in resolve_address(const std::string &hostname, int port);


		class socket
		{
		public:
			socket();
			~socket();

			socket(socket &&sock);
			socket &operator=(socket &&sock);

			socket(const socket &sock);
			socket &operator=(const socket &sock);

		public:
			void connect(const sockaddr_in &addr);

			void send(const char *buf, size_t len);
			size_t recv(char *buf, size_t len);

		public:
			void associate_with_threadpool();

			future<void> send_async(const char *buf, size_t len);
			future<std::string> recv_async();

		private:
			details::socket *sock;
			friend class acceptor;
		};


		class acceptor
		{
		public:
			acceptor();
			~acceptor();

			acceptor(acceptor &&) = delete;
			acceptor &operator=(acceptor &&) = delete;

			acceptor(const acceptor &) = delete;
			acceptor &operator=(const acceptor &sock) = delete;

		public:
			void listen(const sockaddr_in &in);
			socket accept();

		private:
			SOCKET handle;
		};


	}


}


#include "net.inl"
