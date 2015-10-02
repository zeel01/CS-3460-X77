
#include "stdafx.h"

#include "server.h"

namespace multi_thread
{
	void run(std::shared_ptr<cs477::data::database> db)
	{
		std::vector<cs477::thread> thread_list;

		auto db_queue = std::make_shared<cs477::queue<std::pair<cs477::net::socket, cs477::net::http_request>>>();
		thread_list.push_back(cs477::create_thread([db, db_queue]
		{

			for (;;)
			{
				auto pair = db_queue->pop();

				auto sock = std::move(pair.first);
				auto request = std::move(pair.second);

				int status = 200;
				std::string result;

				// Dispatch request to database
				if (request.method == "GET")
				{
					auto stmt = make_query(*db, request);
					result = stmt.execute_query();
				}
				else if (request.method == "POST")
				{
					auto stmt = make_insert(*db, request);
					stmt.execute();
				}
				else if (request.method == "DELETE")
				{
					auto stmt = make_delete(*db, request);
					stmt.execute();
				}
				else
				{
					status = 404;
				}

				// Write the response
				cs477::queue_work([status, result = std::move(result), sock = std::move(sock)]() mutable
				{
					auto rsp = make_response(status, result);
					cs477::net::write_http_response(sock, rsp);
				});
			}
		}));


		auto host = std::make_shared<cs477::net::acceptor>();
		auto addr = cs477::net::resolve_address("penelope", 8080);
		host->listen(addr);


		auto threads = std::thread::hardware_concurrency();
		for (unsigned i = 0; i < threads; i++)
		{
			thread_list.push_back(cs477::create_thread([host, db, db_queue]
			{
				for (;;)
				{
					auto sock = host->accept();

					try
					{
						// Read an http request
						cs477::queue_work([sock = std::move(sock), db_queue] () mutable
						{
							auto request = cs477::net::read_http_request(sock);

							// Enqueue the request
							db_queue->push(std::make_pair(std::move(sock), std::move(request)));
						});
					}
					catch (...)
					{
						// Ignore the error.
					}
				}
			}));
		}

		for (auto &&t : thread_list)
		{
			cs477::join(t);
		}

	}

	void run2(std::shared_ptr<cs477::data::database> db)
	{
		auto threads = std::thread::hardware_concurrency();
		std::vector<cs477::thread> thread_list;

		auto db_queue = std::make_shared<cs477::queue<std::pair<cs477::net::socket, cs477::net::http_request>>>();
		
		for (unsigned i = 0; i < threads; i++)
		{
			thread_list.push_back(cs477::create_thread([db_queue]
			{
				auto db = cs477::data::open("c:\\dev\\2015\\my.db");

				for (;;)
				{
					auto pair = db_queue->pop();

					auto sock = std::move(pair.first);
					auto request = std::move(pair.second);

					int status = 200;
					std::string result;

					// Dispatch request to database
					if (request.method == "GET")
					{
						auto stmt = make_query(*db, request);
						result = stmt.execute_query();
					}
					else if (request.method == "POST")
					{
						auto stmt = make_insert(*db, request);
						stmt.execute();
					}
					else if (request.method == "DELETE")
					{
						auto stmt = make_delete(*db, request);
						stmt.execute();
					}
					else
					{
						status = 404;
					}

					// Write the response
					cs477::queue_work([status, result = std::move(result), sock = std::move(sock)]() mutable
					{
						auto rsp = make_response(status, result);
						cs477::net::write_http_response(sock, rsp);
					});
				}
			}));
		}

		auto host = std::make_shared<cs477::net::acceptor>();
		auto addr = cs477::net::resolve_address("penelope", 8080);
		host->listen(addr);


		for (unsigned i = 0; i < threads; i++)
		{
			thread_list.push_back(cs477::create_thread([host, db, db_queue]
			{
				for (;;)
				{
					auto sock = host->accept();

					try
					{
						// Read an http request
						cs477::queue_work([sock = std::move(sock), db_queue]() mutable
						{
							auto request = cs477::net::read_http_request(sock);

							// Enqueue the request
							db_queue->push(std::make_pair(std::move(sock), std::move(request)));
						});
					}
					catch (...)
					{
						// Ignore the error.
					}
				}
			}));
		}

		for (auto &&t : thread_list)
		{
			cs477::join(t);
		}

	}
}