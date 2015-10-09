
#include "stdafx.h"

#include "server.h"

namespace single_thread
{
	void run(std::shared_ptr<cs477::data::database> db)
	{
		auto host = std::make_shared<cs477::net::acceptor>();
		auto addr = cs477::net::resolve_address("192.168.1.75", 8080);
		host->listen(addr);

		for (;;)
		{
			auto sock = host->accept();

			try
			{
				// Read an http request
				auto request = cs477::net::read_http_request(sock);

				int status = 200;
				std::string result;

				// Dispatch request to database
				//if (request.method == "GET")
				//{
				//	auto stmt = make_query(*db, request);
				//	result = stmt.execute_query();
				//}
				//else if (request.method == "POST")
				//{
				//	auto stmt = make_insert(*db, request);
				//	stmt.execute();
				//}
				//else if (request.method == "DELETE")
				//{
				//	auto stmt = make_delete(*db, request);
				//	stmt.execute();
				//}
				//else
				//{
				//	status = 404;
				//}

				// Write the response
				auto rsp = make_response(status, result);
				cs477::net::write_http_response(sock, rsp);
			}
			catch (...)
			{
				// Ignore the error.
			}
		}

	}
}
