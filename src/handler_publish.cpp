#include "handler_publish.h"
#include "connection.h"
#include "manager_event_stream.h"

handler_publish::handler_publish(handler&& h)
: handler(std::move(h)) {

}

void handler_publish::run(coroutine_handler yield) {
    boost::system::error_code error;

    auto& q = parse_query();
    auto  c = manager_event_stream::get().write(q["topic"], q["event"], req_.body(), yield);

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