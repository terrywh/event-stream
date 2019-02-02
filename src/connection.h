#pragma
#include "vendor.h"
#include "coroutine.h"

class connection: public std::enable_shared_from_this<connection>, public boost::asio::coroutine {
public:
	connection();
	~connection();
	void close();
	void run(coroutine_handler yield);
public:
	boost::asio::io_context::strand        rr_;
	boost::asio::io_context::strand        rw_;
	boost::asio::ip::tcp::socket           socket_;
private:
	bool                                   closed_ = false;
	boost::beast::multi_buffer             rbuf_;
};
