// dbp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "../httpd/shared.h"
#include "image.h"



std::shared_ptr<cs477::shm_pool> rq_pool;
std::shared_ptr<cs477::bounded_queue<message, 1024>> rq_queue;
std::shared_ptr<cs477::bounded_queue<message, 1024>> rp_queue;

std::shared_ptr<cs477::data::database> db;


int main()
{
	using namespace cs477;

	// Create shared resources.

	rq_pool = std::make_shared<cs477::shm_pool>();
	rq_pool->create("httpd-rq-pool", 4096, 16384);

	rq_queue = std::make_shared<cs477::bounded_queue<message, 1024>>();
	rq_queue->create("httpd-rq-queue");

	rp_queue = std::make_shared<cs477::bounded_queue<message, 1024>>();
	rp_queue->create("httpd-rp-queue");


	// Create private resources

	for (;;)
	{
		// Read a request
		auto msg = rq_queue->read();

		// Parse the actual content.
		buffer rq_buf{ rq_pool, msg };
		auto request = net::read_http_request(rq_buf.data(), rq_buf.size());

		// TODO: Handle the request.
		net::http_response rsp;
		if (request.method == "GET")
		{
			rsp = get_image(request);
		}
		else if (request.method == "POST")
		{
			rsp = post_image(request);
		}

		auto response = net::write_http_response(rsp);

		// Create a response buffer and copy the response text into it.
		// TODO: Maybe use another pool?
		buffer rp_buf{ rq_pool };
		rp_buf.allocate(response.length());
		memcpy(rp_buf.data(), response.c_str(), response.length());

		// Write the response back to the server.
		rp_queue->write(rp_buf.make_message());
	}

	return 0;
}

