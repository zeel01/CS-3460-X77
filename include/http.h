
#pragma once

#include "cs477.h"


namespace cs477
{

	namespace net
	{

		class http_request
		{
		public:
			std::string method;
			std::string url;
			std::vector<std::pair<std::string, std::string>> headers;
			std::string body;
		};

		http_request read_http_request(socket &sock);

		class http_response
		{
		public:
			int status;
			std::string message;
			std::vector<std::pair<std::string, std::string>> headers;
			std::string body;
		};

		void write_http_response(socket &sock, const http_response &rsp);

	}

}
