// httpd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "shared.h"
#include "client.h"


// Globals

std::shared_ptr<cs477::shm_pool> rq_pool;
std::shared_ptr<cs477::bounded_queue<message, 1024>> rq_queue;
std::shared_ptr<cs477::bounded_queue<message, 1024>> rp_queue;


void create_db_process();
void create_fs_process();

cs477::process fsp;
cs477::process dbp;

int main()
{
	using namespace cs477;

	net::initialize();


	// Create shared resources.

	rq_pool = std::make_shared<cs477::shm_pool>();
	rq_pool->create("httpd-rq-pool", 4096, 16384);

	rq_queue = std::make_shared<cs477::bounded_queue<message, 1024>>();
	rq_queue->create("httpd-rq-queue");

	rp_queue = std::make_shared<cs477::bounded_queue<message, 1024>>();
	rp_queue->create("httpd-rp-queue");


	//// Create the child processes:
	//create_db_process();
	//create_fs_process();

	// Start the server

	net::acceptor server;
	auto ep = net::resolve_address("localhost", 8080);
	server.listen(ep);

	
	for (;;)
	{
		// Accept an incoming connection
		auto sock = server.accept();
		auto client = std::make_shared<client_context>(sock);
	
		// Read the http request
		auto msg = client->read_request(rq_pool);
		
		// Send the request to a processor.
		rq_queue->write(msg);

		// Read the response from the processor
		msg = rp_queue->read();
		
		// TODO: Figure out which shm_pool msg refers to.
		// For now, assume rq_pool.
		auto pool = rq_pool;
		auto buf = buffer{ pool, msg };
		client->write_response(buf);
	}

	net::finalize();

    return 0;
}



void create_db_process()
{
	dbp = cs477::create_process("..\\Debug\\dbp.exe", "");
	dbp->wait().then([] (auto f)
	{
		create_db_process();
	});
}

void create_fs_process()
{
	fsp = cs477::create_process("..\\Debug\\fsp.exe", "");
	fsp->wait().then([](auto f)
	{
		create_fs_process();
	});
}

