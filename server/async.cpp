
#include "stdafx.h"

#include "server.h"


namespace async
{
	void socket_handler(cs477::net::socket sock)
	{
		// Read an http request
		auto f = cs477::net::read_http_request_async(sock);

		f.then([sock](auto f)
		{
			auto rq = f.get();

			int status = 200;
			std::string result;

			// Dispatch request to database
			//if (rq.method == "GET")
			//{
			//	auto stmt = make_query(*db, rq);
			//	result = stmt.execute_query();
			//}
			//else if (rq.method == "POST")
			//{
			//	auto stmt = make_insert(*db, rq);
			//	stmt.execute();
			//}
			//else if (rq.method == "DELETE")
			//{
			//	auto stmt = make_delete(*db, rq);
			//	stmt.execute();
			//}
			//else
			//{
			//	status = 404;
			//}

			// Write the response
			auto rsp = make_response(200, {});
			cs477::net::write_http_response_async(sock, rsp);

			return 0;
		});
	}


	void run(std::shared_ptr<cs477::data::database> db)
	{
		auto host = std::make_shared<cs477::net::acceptor>();
		auto addr = cs477::net::resolve_address("192.168.1.75", 8080);
		//auto addr = cs477::net::resolve_address("localhost", 8080);
		host->listen(addr);

		host->accept_async(socket_handler);

		for (;;)
		{

		}

	}
}
