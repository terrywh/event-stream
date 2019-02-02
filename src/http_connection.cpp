#include "http_connection.h"
#include "config.h"
#include "http_handler.h"
#include "http_handler_event_stream.h"
#include "http_handler_publish.h"

http_connection::http_connection()
: rr_(config::get().context)
, rw_(config::get().context)
, socket_(config::get().context) {

}

http_connection::~http_connection() {

}


void http_connection::close() {
	if(closed_) return;
	closed_ = true;
	socket_.close();
}

void http_connection::run(coroutine_handler yield) {
	boost::system::error_code  error;
	std::shared_ptr<http_handler> handler;
	do {
		handler.reset(new class http_handler(shared_from_this()));
		boost::beast::http::async_read(socket_, rbuf_, handler->req_, yield[error]);
		if (error) {
			if (error != boost::asio::error::operation_aborted && error != boost::asio::error::connection_reset
				&& error != boost::asio::error::eof && error != boost::beast::http::error::end_of_stream) {
				std::cerr << "(ERROR) [connection] failed to read request: (" << error.value() << ") "
						  << error.message() << std::endl;
			}
			close();
			return;
		}
		auto target = handler->req_.target();
		if (target.substr(0, 7).compare("/v1/sub") == 0) {
			handler.reset(new http_handler_event_stream(std::move(*handler)));
		} else if (target.substr(0, 7).compare("/v1/pub") == 0) {
			handler.reset(new http_handler_publish(std::move(*handler)));
		} else if (target.substr(0, 11).compare("/v1/version") == 0) {
			handler->respond_version(yield);
			return;
		} else if (target.substr(0, 12).compare("/favicon.ico") == 0) {
			handler->respond_not_found(yield);
			return;
		}
		handler->run(yield);
	}while(handler->req_.keep_alive() && handler->res_.keep_alive());
}
