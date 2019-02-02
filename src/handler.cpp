#include "handler.h"
#include "config.h"
#include "connection.h"

handler::handler(std::shared_ptr<connection> cc)
: connection_(cc)
, res_sr_(res_){
    res_.set("X-Node", config::get().node);
}

handler::handler(handler&& h)
: connection_(std::move(h.connection_))
, req_(std::move(h.req_))
, res_(std::move(h.res_))
, res_sr_(res_)
{

}

void handler::run(coroutine_handler yield) {
    // 默认实现返回 404
    respond_not_found(yield);
}

std::map<std::string, std::string>& handler::parse_query() {
    if(!query_)
    std::call_once(parse_query_once, [this] () {
        std::string_view sv(req_.target());
        auto x = sv.find_first_of('?');
        query_.reset(new std::map<std::string, std::string>(parse_query(sv.substr(x + 1))));
    });
    return *query_;
}

void handler::respond_not_found(coroutine_handler& yield) {
    res_.keep_alive(false);
    res_.result(boost::beast::http::status::not_found);
    boost::beast::http::async_write(connection_->socket_, res_, yield);
}

void handler::respond_bad_request(coroutine_handler& yield) {
    res_.keep_alive(false);
    res_.result(boost::beast::http::status::bad_request);
    boost::beast::http::async_write(connection_->socket_, res_, yield);
}

void handler::respond_version(coroutine_handler& yield) {
    res_.body() = VERSION_STR;
    res_.prepare_payload();
    boost::beast::http::async_write(connection_->socket_, res_, yield);
}

char handler::hex2bin(char x) {
    if(x >= 'a' && x <= 'f') {
        return x - 'a' + 10;
    }else if(x >= 'A' && x <= 'F') {
        return x - 'A' + 10;
    }else if(x >= '0' && x <= '9') {
        return x - '0';
    }else{
        return 0;
    }
}

void handler::decode_uri_inplace(std::string& str) {
    char* data = str.data();
    auto x = 0;
    for(auto i=0;i<str.size();++i) {
        if(data[i] == ' ') {
            data[x] = '+';
            ++x;
        }else if(data[i] == '%') {
            data[x] = hex2bin(data[i+1]) << 4 | hex2bin(data[i+2]);
            ++x;
            i+=2;
        }else{
            data[x] = data[i];
            ++x;
        }
    }
    str.resize(x);
}

std::map<std::string, std::string> handler::parse_query(std::string_view str) {
    std::map<std::string, std::string> query;
    parser::separator_parser<std::string, std::string> parser('\0','\0','=','\0','\0','&', [&query] (std::pair<std::string, std::string> entry) {
        handler::decode_uri_inplace(entry.second);
        query.insert(std::move(entry));
    });
    parser.parse(str.data(), str.size());
    parser.end();
    return std::move(query);
}
