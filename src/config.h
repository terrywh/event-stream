#pragma once
#include "vendor.h"

class config {
public:
	boost::asio::io_context        context;
	boost::asio::ip::tcp::endpoint address;
	std::string                    node;
	static config& get();
	bool init(int argc, char* argv[]);
	bool                           closing = false;
};
