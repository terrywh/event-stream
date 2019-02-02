#include "server.h"
#include "config.h"
#include "connection.h"
#include "coroutine.h"

server::server()
:acptr_(config::get().context, config::get().address) {

	std::cout << "(INFO) [server] listening at: " << config::get().address << std::endl;
}

void server::run(coroutine_handler yield) {
	for(;!config::get().closing;) {
		auto ss = std::make_shared<connection>();
		acptr_.async_accept(ss->socket_, yield);
		coroutine::start(ss->rr_, std::bind(&connection::run, ss, std::placeholders::_1));
	}
}
