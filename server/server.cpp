// server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "server.h"


int main()
{
	cs477::net::initialize();

	auto db = cs477::data::open("c:\\dev\\2015\\my.db");
	db->execute("create table if not exists data (key text, value text)");

	single_thread::run(db);
	//multi_thread::run(db);

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

