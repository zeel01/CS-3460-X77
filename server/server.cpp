// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../include/cs477.h"

cs477::data::statement make_query(cs477::data::database &db, const cs477::net::http_request &request);
cs477::data::statement make_insert(cs477::data::database &db, const cs477::net::http_request &request);
cs477::data::statement make_delete(cs477::data::database &db, const cs477::net::http_request &request);

cs477::net::http_response make_response(int status, const std::string &json);


int main()
{
	cs477::net::initialize();

	auto db = cs477::data::open("c:\\dev\\2015\\my.db");
	db.execute("create table if not exists data (key text, value text)");

	cs477::net::acceptor host;
	auto addr = cs477::net::resolve_address("localhost", 8080);
	host.listen(addr);

	cs477::queue_work([]
	{
		cs477::net::socket sock;
		sock.connect(cs477::net::resolve_address("localhost", 8080));

		auto rq =
			"GET /test HTTP/1.1\r\n"
			"\r\n";

		sock.send(rq, strlen(rq));

		std::string rsp;
		rsp.resize(65536);

		auto len = sock.recv(&rsp.front(), 65536);
		rsp.resize(len);

		printf("%s\n", rsp.c_str());

	});

	for (;;)
	{
		auto sock = host.accept();

		try
		{
			// Read an http request
			auto request = cs477::net::read_http_request(sock);

			int status = 200;
			std::string result;

			// Dispatch request to database
			if (request.method == "GET")
			{
				auto stmt = make_query(db, request);
				result = stmt.execute_query();
			}
			else if (request.method == "POST")
			{
				auto stmt = make_insert(db, request);
				stmt.execute();
			}
			else if (request.method == "DELETE")
			{
				auto stmt = make_delete(db, request);
				stmt.execute();
			}
			else
			{
				status = 404;
			}

			// Write the response
			auto rsp = make_response(status, result);
			cs477::net::write_http_response(sock, rsp);
			
		}
		catch (...)
		{
			// Ignore the error.
		}
	}


	cs477::net::finalize();
	return 0;
}


cs477::data::statement make_query(cs477::data::database &db, const cs477::net::http_request &request)
{
	return db.new_statement("select * from data");
}

cs477::data::statement make_insert(cs477::data::database &db, const cs477::net::http_request &request)
{
	return db.new_statement("insert into data values('key', 'value')");
}

cs477::data::statement make_delete(cs477::data::database &db, const cs477::net::http_request &request)
{
	return db.new_statement("delete from data where key='key'");
}

cs477::net::http_response make_response(int status, const std::string &json)
{
	cs477::net::http_response rsp;
	rsp.status = status;
	switch (status)
	{
	case 200:
		rsp.message = "Ok";
		break;
	case 404:
		rsp.message = "Not Found";
		break;
	case 500:
		rsp.message = "Internal Error";
		break;
	}

	if (status == 200)
	{
		rsp.body = json;
		rsp.headers.emplace_back("Content-Type", "application/json");
	}

	return rsp;
}

