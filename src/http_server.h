#pragma once
#include "vendor.h"
#include "coroutine.h"

class http_server: public std::enable_shared_from_this<http_server> {
public:
	http_server();
	void run(coroutine_handler yield);
private:
	boost::asio::ip::tcp::acceptor acptr_;
};
