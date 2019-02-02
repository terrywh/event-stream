#pragma once
#include "vendor.h"
#include "coroutine.h"

class http_connection;
class http_handler: public std::enable_shared_from_this<http_handler> {
public:
    http_handler(std::shared_ptr<http_connection> cc);
    http_handler(http_handler&& h);
    virtual ~http_handler() = default;
    virtual void run(coroutine_handler yield);
protected:
    std::map<std::string, std::string>& parse_query();

    void respond_not_found(coroutine_handler& yield);
    void respond_bad_request(coroutine_handler& yield);
    void respond_version(coroutine_handler& yield);

    std::shared_ptr<http_connection>                                         connection_;
    boost::beast::http::request<boost::beast::http::string_body>             req_;
    std::shared_ptr<std::map<std::string, std::string>>                      query_;
    boost::beast::http::response<boost::beast::http::string_body>            res_;
    boost::beast::http::response_serializer<boost::beast::http::string_body> res_sr_;

    static char hex2bin(char x);
    static void decode_uri_inplace(std::string& str);
    static std::map<std::string, std::string> parse_query(std::string_view str);
private:
    std::once_flag parse_query_once;

    friend class http_connection;
};
