#pragma once
#include "vendor.h"
#include "coroutine.h"

class connection;
class server: public std::enable_shared_from_this<server> {
public:
	server();
	void run(coroutine_handler yield);
private:
	boost::asio::ip::tcp::acceptor acptr_;
};
