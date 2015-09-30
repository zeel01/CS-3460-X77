
#include "../http_parser/http_parser.h"
#pragma comment(lib, "http_parser.lib")


namespace cs477
{

	namespace net
	{

		struct http_request_parse_state
		{
			http_request rq;

			enum
			{
				url,
				name,
				value,
				body,
				done,
			} state;
		};

		int on_request_begin_message(http_parser *parser)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->rq.method = http_method_str(http_method(parser->method));
			return 0;
		}

		int on_request_url(http_parser *parser, const char *at, size_t length)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->rq.url.append(at, length);
			state->state = http_request_parse_state::url;
			return 0;
		}

		int on_request_header_field(http_parser *parser, const char *at, size_t length)
		{
			auto state = (http_request_parse_state *)parser->data;
			if (state->state != http_request_parse_state::name)
			{
				state->rq.headers.emplace_back();
				state->state = http_request_parse_state::name;
			}
			state->rq.headers.back().first.append(at, length);
			return 0;
		}

		int on_request_header_value(http_parser *parser, const char *at, size_t length)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->rq.headers.back().second.append(at, length);
			state->state = http_request_parse_state::value;
			return 0;
		}

		int on_request_headers_complete(http_parser *parser)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->state = http_request_parse_state::body;
			return 0;
		}

		int on_request_body(http_parser *parser, const char *at, size_t length)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->rq.body.append(at, length);
			return 0;
		}

		int on_request_message_complete(http_parser *parser)
		{
			auto state = (http_request_parse_state *)parser->data;
			state->state = http_request_parse_state::done;
			return 0;
		}

		inline http_request read_http_request(socket &sock)
		{
			http_request_parse_state state;

			http_parser_settings callbacks = { 0 };
			callbacks.on_message_begin = on_request_begin_message;
			callbacks.on_url = on_request_url;
			callbacks.on_header_field = on_request_header_field;
			callbacks.on_header_value = on_request_header_value;
			callbacks.on_headers_complete = on_request_headers_complete;
			callbacks.on_body = on_request_body;
			callbacks.on_message_complete = on_request_message_complete;

			http_parser parser;
			http_parser_init(&parser, HTTP_REQUEST);
			parser.data = &state;

			std::string str;
			str.resize(65536);

			for (;;)
			{
				auto len = sock.recv(&str.front(), 65536);
				auto parsed = http_parser_execute(&parser, &callbacks, str.c_str(), len);
				if (parsed != len)
				{
					throw std::exception();
				}

				if (state.state == http_request_parse_state::done)
				{
					break;
				}
			}

			return std::move(state.rq);
		}

		inline void write_http_response(socket &sock, const http_response &rsp)
		{
			char line[128];

			std::string text;

			sprintf_s(line, "HTTP/1.1 %d %s\r\n", rsp.status, rsp.message.c_str());
			text.append(line);

			if (rsp.body.length())
			{
				sprintf_s(line, "Content-Length : %d\r\n", rsp.body.length());
				text.append(line);
			}

			for (auto &hdr : rsp.headers)
			{
				sprintf_s(line, "%s : %s\r\n", hdr.first.c_str(), hdr.second.c_str());
				text.append(line);
			}

			text.append("\r\n");

			text.append(rsp.body);

			sock.send(text.c_str(), text.length());
		}

	}
}