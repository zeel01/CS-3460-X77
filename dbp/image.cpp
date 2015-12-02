
#include "stdafx.h"

#include "image.h"

extern std::shared_ptr<cs477::data::database> db;



cs477::net::http_response get_image(const cs477::net::http_request &request)
{
	// TODO: Parse url
	return get_images();
}

cs477::net::http_response get_images()
{
	auto ids = db->execute_query("select id from image");

	cs477::net::http_response response;
	response.status = 200;
	response.message = "Ok";
	response.headers.push_back({ "Content-Type", "application/json" });
	response.body = ids;

	return response;
}

cs477::net::http_response get_image(int id)
{
	auto stmt = db->new_statement("select path from image where id=?");
	
	char id_string[12];
	sprintf_s(id_string, "%d", id);
	stmt.bind(0, id_string);
	
	auto path = stmt.execute_query();
	
	cs477::net::http_response response;
	response.status = 200;
	response.message = "Ok";
	response.headers.push_back({ "Content-Type", "application/json" });
	response.body = path;

	return response;
}

cs477::net::http_response post_image(const cs477::net::http_request &request)
{
	// TODO: Parse name from url
	return post_image("");
}

cs477::net::http_response post_image(const std::string &name)
{

	auto stmt = db->new_statement("insert into image values (?)");
	stmt.bind(0, name.c_str());
	stmt.execute();

	stmt = db->new_statement("select id from image where name=?");
	stmt.bind(0, name.c_str());
	auto id_string = stmt.execute_query();
	// TODO: Parse response
	auto id = 0;

	cs477::net::http_response response;
	response.status = 201;
	response.message = "Created";

	char location[64];
	sprintf_s(location, "/image/%d", id);
	response.headers.push_back({ "Location", location });

	return response;
}

