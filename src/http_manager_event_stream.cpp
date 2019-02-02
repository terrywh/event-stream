#include "http_manager_event_stream.h"
#include "coroutine.h"
#include "http_connection.h"
#include "http_handler_event_stream.h"

http_manager_event_stream& http_manager_event_stream::get() {
	static http_manager_event_stream sm;
	return sm;
}

int http_manager_event_stream::write(const std::string& topic, const std::string& event, const std::string& data, coroutine_handler& yield) {
	auto r = topic_.find(topic);
	if(r == topic_.end()) return 0;
	// 不支持多行数据
//	if(data.find_last_of('\n') != std::string::npos) return 0;
    auto c = 0;
    boost::system::error_code error;
	for(auto i=r->second.begin(); i!=r->second.end(); ++i) {
		// 未设置事件过滤或包含指定的事件
		if(event.empty() || (*i)->event_.empty() || (*i)->event_.count(event) > 0) {
			(*i)->write(event, data, yield[error]);
			if(error) error = boost::system::error_code();
			else ++c;
		}
	}
	return c;
}

void http_manager_event_stream::enter(std::shared_ptr<http_handler_event_stream> es) {
	for(auto i = es->topic_.begin(); i!= es->topic_.end(); ++i) {
		topic_[*i].insert(es);
	}
}

void http_manager_event_stream::leave(std::shared_ptr<http_handler_event_stream> es) {
	for(auto i = es->topic_.begin(); i!= es->topic_.end(); ++i) {
		auto ref = topic_[*i];
		if(ref.erase(es) && ref.empty()) {
			topic_.erase(*i);
		}
	}
}
