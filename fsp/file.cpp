
#include "stdafx.h"

#include "file.h"

#include <objbase.h>

cs477::net::http_response save_file(const std::string &str)
{
	GUID guid;
	CoCreateGuid(&guid);
	char name[64];
	sprintf_s(name, "%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	cs477::write_file(name, str);

	cs477::net::http_response response;
	response.status = 201;
	response.message = "Created";

	char location[64];
	sprintf_s(location, "/image/%s", name);
	response.headers.push_back({ "Location", location });

	return response;
}

cs477::net::http_response load_file(const std::string &name)
{
	auto str = cs477::read_file(name.c_str());

	cs477::net::http_response response;
	response.status = 200;
	response.message = "Ok";
	response.body = std::move(str);

	return response;
}