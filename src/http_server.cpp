#include "http_server.h"
#include "config.h"
#include "http_connection.h"
#include "coroutine.h"

http_server::http_server()
:acptr_(config::get().context, config::get().address) {

	std::cout << "(INFO) [server] listening at: " << config::get().address << std::endl;
}

void http_server::run(coroutine_handler yield) {
	for(;!config::get().closing;) {
		auto ss = std::make_shared<http_connection>();
		acptr_.async_accept(ss->socket_, yield);
		coroutine::start(ss->rr_, std::bind(&http_connection::run, ss, std::placeholders::_1));
	}
}
