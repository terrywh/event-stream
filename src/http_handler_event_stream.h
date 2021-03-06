#pragma once
#include "vendor.h"
#include "http_handler.h"

class http_handler_event_stream: public http_handler {
public:
    explicit http_handler_event_stream(http_handler&& h);
    virtual void run(coroutine_handler yield) override;
    void write(const std::string& evt, const std::string& dat, coroutine_handler& yield);
    void close();

    std::set<std::string> topic_;
    std::set<std::string> event_;
private:
    void parse_topic(std::string_view topic);
    void parse_event(std::string_view event);

    bool close_ = false;
};
