
#include "stdafx.h"

#include "client.h"



client_context::client_context(cs477::net::socket sock)
	: _sock(sock)
{
	memset(&_parser, 0, sizeof(parser));
	_parser.on_message_complete = on_message_complete;
	_parser.data = this;
}

client_context::~client_context()
{
	rq_buf.deallocate();
}

message client_context::read_request(std::shared_ptr<cs477::shm_pool> pool)
{
	_parser.is_done = false;
	http_parser_init(&_parser, HTTP_REQUEST);

	buffer buf{ pool };
	buf.allocate(4096);

	uint32_t offset = 0;
	for (;;)
	{
		auto data = buf.data();
		auto max = buf.size();

		auto recvd = _sock.recv(data + offset, max - offset);
		
		auto parsed = http_parser_execute(&_parser, &_parser, data + offset, max - offset);
		if (parsed == 0 || _parser.http_errno)
		{
			throw std::exception("Parse failed");
		}
		else if (_parser.is_done)
		{
			offset += recvd;
			buf.shrink(offset);
			
			rq_buf = buf;
			return rq_buf.make_message();
		}
		else if (parsed == recvd)
		{
			// Read to the end of the buffer.  Reallocate the buffer and continue reading.
			buf.allocate(2 * buf.size());
			offset += recvd;
		}
		else
		{
			// What else could happen?
			throw std::exception("Invalid state");
		}
	}
}

void client_context::write_response(const buffer &buf)
{
	auto data = buf.data();
	auto len = buf.size();

	_sock.send(data, len);
}

int client_context::on_message_complete(http_parser *parser)
{
	auto c = (client_context *)parser->data;
	c->_parser.is_done = true;
	return 0;
}