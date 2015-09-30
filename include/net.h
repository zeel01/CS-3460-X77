
#pragma once

#include "cs477.h"

namespace cs477
{

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

			socket(const socket &) = delete;
			socket &operator=(const socket &sock) = delete;

		public:
			void connect(const sockaddr_in &addr);

			void send(const char *buf, size_t len);
			size_t recv(char *buf, size_t len);

		private:
			SOCKET handle;

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

