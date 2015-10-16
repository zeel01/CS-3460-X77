// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "server.h"

int main()
{
	cs477::net::initialize();

	auto f = cs477::read_file_async("C:\\Dev\\2015\\CS-3460-X77\\index.json");
	auto str = f.get();

	auto f2= cs477::write_file_async("C:\\Dev\\2015\\test.json", str);
	f2.wait();

	auto db = cs477::data::open("c:\\dev\\2015\\my.db");
	db->execute("create table if not exists data (key text, value text)");

	//cs477::queue_work([]
	//{
	//	for (int i = 0; i < 10; i++)
	//	{
	//		cs477::net::socket sock;
	//		sock.connect(cs477::net::resolve_address("localhost", 8080));
	//		std::string str =
	//			"GET /test HTTP/1.1\r\n"
	//			"\r\n";
	//		sock.send(str.c_str(), str.length());

	//		str.resize(1000);
	//		auto len = sock.recv(&str.front(), 1000);
	//		str.resize(len);

	//		printf("%s\n", str.c_str());
	//	}
	//});


	//auto addr = cs477::net::resolve_address("10.240.92.63", 8080);
	auto addr = cs477::net::resolve_address("localhost", 8080);
	//auto addr = cs477::net::resolve_address("10.0.255.32", 8080);

	//single_thread::run(db, addr);
	//multi_thread::run2(db);
	async::run(addr, db);

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
		if (json.length())
		{
			rsp.body = json;
			rsp.headers.emplace_back("Content-Type", "application/json");
		}
	}

	return rsp;
}

