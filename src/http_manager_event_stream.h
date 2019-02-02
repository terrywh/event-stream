#pragma once
#include "vendor.h"

class coroutine_handler;
class http_handler_event_stream;
class http_manager_event_stream {
public:
	static http_manager_event_stream& get();
	int  write(const std::string& topic, const std::string& event, const std::string& data, coroutine_handler& ch);
	void enter(std::shared_ptr<http_handler_event_stream> es);
	void leave(std::shared_ptr<http_handler_event_stream> es);
private:
    std::map<std::string, std::set<std::shared_ptr<http_handler_event_stream>>> topic_;
};
