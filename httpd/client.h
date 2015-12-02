#pragma once

#include "shared.h"


class client_context
{
public:
	client_context(cs477::net::socket sock);
	~client_context();

	message read_request(std::shared_ptr<cs477::shm_pool> pool);
	void write_response(const buffer &buf);


private:
	cs477::net::socket _sock;
	buffer rq_buf;

	struct parser : public http_parser, http_parser_settings
	{
		bool is_done;

	} _parser;

	static int on_message_complete(http_parser *parser);
};