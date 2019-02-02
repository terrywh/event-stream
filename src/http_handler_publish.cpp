#include "http_handler_publish.h"
#include "http_connection.h"
#include "http_manager_event_stream.h"

http_handler_publish::http_handler_publish(http_handler&& h)
: http_handler(std::move(h)) {

}

void http_handler_publish::run(coroutine_handler yield) {
    boost::system::error_code error;

    auto& q = parse_query();
    auto  c = http_manager_event_stream::get().write(q["topic"], q["event"], req_.body(), yield);
	// 响应对应实际发送目标数量
    res_.body().assign( (boost::format(R"JSON({"data":%d})JSON") % c).str() );
    res_.prepare_payload();
    boost::beast::http::async_write(connection_->socket_, res_, yield[error]);
    if(error) {
        if(error != boost::asio::error::operation_aborted) {
            std::cerr << "(ERROR) [handler/publish] failed to publish event: (" << error.value() << ") " << error.message() << std::endl;
            connection_->close();
        }
        return;
    }

}
