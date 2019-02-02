#include "http_handler_event_stream.h"
#include "coroutine.h"
#include "http_connection.h"
#include "http_manager_event_stream.h"

http_handler_event_stream::http_handler_event_stream(http_handler&& h)
: http_handler(std::move(h)) {

}

void http_handler_event_stream::write(const std::string& evt, const std::string& dat, coroutine_handler& ch) {
    coroutine::start(connection_->rw_, [&ch, evt, dat, this, ref = shared_from_this()] (coroutine_handler yield) {
        boost::system::error_code error;
        if(evt.empty()) {
            boost::asio::async_write(connection_->socket_,
                                     boost::beast::http::make_chunk(boost::asio::buffer("data: ", 6)),
                                     yield[error]);
        }else{
            auto str = (boost::format("event: %1%\ndata: ") % evt).str();
            boost::asio::async_write(connection_->socket_,
                                     boost::beast::http::make_chunk(boost::asio::buffer(str.c_str(), str.size())),
                                     yield[error]);
        }
        if(error) {
            ch.resume(error);
            return;
        }
        boost::asio::async_write(connection_->socket_,
                                 boost::beast::http::make_chunk(boost::asio::buffer(dat.c_str(), dat.size())),
                                 yield[error]);
        if(error) {
            ch.resume(error);
            return;
        }
        boost::asio::async_write(connection_->socket_,
                                boost::beast::http::make_chunk(boost::asio::buffer("\n\n", 2)), yield[error]);
        ch.resume(error);
    });
    boost::system::error_code error;
    ch.yield(error);
    if(error) close();
}

void http_handler_event_stream::close() {
    if(close_) return;
    close_ = true;
    connection_->close();
    http_manager_event_stream::get().leave(
            std::dynamic_pointer_cast<http_handler_event_stream>(shared_from_this()));
}

void http_handler_event_stream::parse_topic(std::string_view topic) {
    if(topic[0] == '[') {
        // 允许使用数组形式订阅多个 TOPIC 数据
        json v;
        try {
            v = json::parse(topic);
            if (!v.is_array()) return;
            for (auto i = v.begin(); i != v.end(); ++i) {
                topic_.insert((*i).get<std::string>());
            }
        } catch (const std::exception &ex) {
            std::cerr << "(EXCEPTION) [handler/event_stream] failed to parse topic: " << ex.what() << std::endl;
            return;
        }
    }else{
        topic_.insert(std::string(topic));
    }
}

void http_handler_event_stream::parse_event(std::string_view event) {
    if(event[0] == '[') {
        // 允许使用数组形式订阅多个 TOPIC 数据
        json v;
        try {
            v = json::parse(event);
            if (!v.is_array()) return;
            for (auto i = v.begin(); i != v.end(); ++i) {
                event_.insert((*i).get<std::string>());
            }
        } catch (const std::exception &ex) {
            std::cerr << "(EXCEPTION) [handler/event_stream] failed to parse event: " << ex.what() << std::endl;
            return;
        }
    }else{
        event_.insert(std::string(event));
    }
}

void http_handler_event_stream::run(coroutine_handler yield) {
    boost::system::error_code error;
    // 解析参数
    auto& q = parse_query();
    if(q.count("topic")) {
        parse_topic(q["topic"]);
    }else{
        respond_bad_request(yield);
        return;
    }
    if(q.count("event")) {
        parse_event(q["event"]);
    }

    res_.keep_alive(false);
    res_.chunked(true);
    res_.set(boost::beast::http::field::content_type, "text/event-stream");
    res_.set(boost::beast::http::field::cache_control, "no-cache");
    boost::beast::http::async_write_header(connection_->socket_, res_sr_, yield[error]);
    if(error) {
        if(error != boost::asio::error::operation_aborted) {
            std::cerr << "(ERROR) [handler/event_stream] failed to write stream header: (" << error.value() << ") " << error.message() << std::endl;
            close();
        }
        return;
    }
    // 将会话计入管理器中
    http_manager_event_stream::get().enter(
            std::dynamic_pointer_cast<http_handler_event_stream>(shared_from_this()));
    connection_->socket_.async_wait(boost::asio::ip::tcp::socket::wait_read, yield[error]);
    close();
}
