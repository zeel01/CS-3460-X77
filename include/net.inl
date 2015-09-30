

namespace cs477
{

	namespace net
	{

		inline void initialize()
		{
			WSADATA data;
			WSAStartup(MAKEWORD(2, 2), &data);
		}

		inline void finalize()
		{
			WSACleanup();
		}

		inline sockaddr_in resolve_address(const std::string &hostname, int port)
		{
			char service[12];
			sprintf_s(service, "%d", port);

			addrinfo hint = { 0 };
			hint.ai_family = AF_INET;

			addrinfo *info = nullptr;
			int err = getaddrinfo(hostname.c_str(), service, &hint, &info);
			if (err == SOCKET_ERROR)
			{
				throw std::exception();
			}

			auto addr = *(sockaddr_in *)info->ai_addr;
			freeaddrinfo(info);

			return addr;
		}





		inline socket::socket()
			: handle(INVALID_SOCKET)
		{
		}

		inline socket::~socket()
		{
			if (handle != INVALID_SOCKET)
			{
				closesocket(handle);
			}
		}

		inline socket::socket(socket &&sock)
			: handle(sock.handle)
		{
			sock.handle = INVALID_SOCKET;
		}

		inline socket &socket::operator=(socket &&sock)
		{
			std::swap(handle, sock.handle);
			return *this;
		}

		inline void socket::connect(const sockaddr_in &addr)
		{
			handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			auto err = ::connect(handle, (sockaddr *)&addr, sizeof(sockaddr_in));
			if (err == SOCKET_ERROR)
			{
				throw std::exception();
			}
		}

		inline void socket::send(const char *buf, size_t len)
		{
			auto ptr = buf;
			auto end = buf + len;

			while (ptr < end)
			{
				auto bytes = static_cast<int>(min(end - ptr, 1048576));
				auto sent = ::send(handle, ptr, bytes, 0);
				if (sent == SOCKET_ERROR || sent == 0)
				{
					throw std::exception();
				}
				ptr += sent;
			}
		}

		inline size_t socket::recv(char *buf, size_t len)
		{
			auto recvd = ::recv(handle, buf, static_cast<int>(len), 0);
			if (recvd == SOCKET_ERROR || recvd == 0)
			{
				throw std::exception();
			}

			return static_cast<size_t>(recvd);
		}


		inline acceptor::acceptor()
			: handle(INVALID_SOCKET)
		{
		}

		inline acceptor::~acceptor()
		{
			if (handle != INVALID_SOCKET)
			{
				closesocket(handle);
			}
		}

		inline void acceptor::listen(const sockaddr_in &addr)
		{
			handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			auto err = ::bind(handle, (sockaddr *)&addr, sizeof(sockaddr_in));
			if (err == SOCKET_ERROR)
			{
				throw std::exception();
			}

			err = ::listen(handle, SOMAXCONN);
			if (err == SOCKET_ERROR)
			{
				throw std::exception();
			}
		}

		inline socket acceptor::accept()
		{
			socket sock;
			sock.handle = ::accept(handle, nullptr, 0);
			if (sock.handle == INVALID_SOCKET)
			{
				throw std::exception();
			}

			return sock;
		}

	}


}

