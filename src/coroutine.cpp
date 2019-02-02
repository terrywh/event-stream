#include "coroutine.h"

coroutine_handler::coroutine_handler() {}

coroutine_handler::coroutine_handler(std::shared_ptr<coroutine> co)
: co_(co) {}

coroutine_handler& coroutine_handler::operator[](boost::system::error_code& error) {
    error_.reset(&error, boost::null_deleter());
    return *this;
}

void coroutine_handler::resume(const boost::system::error_code& error, std::size_t size) {
    if(error_) *error_ = error;
    size_ = size;

    resume();
}

void coroutine_handler::reset() {
    co_.reset();
    error_.reset();
    size_ = 0;
}

void coroutine_handler::reset(std::shared_ptr<coroutine> co) {
    co_ = co;
    error_.reset();
    size_ = 0;
}

void coroutine_handler::yield() {
    coroutine::current.reset();
    co_->c2_ = std::move(co_->c2_).resume();
}

void coroutine_handler::yield(boost::system::error_code& error) {
    error_.reset(&error, boost::null_deleter());
    yield();
}

void coroutine_handler::resume() {
    coroutine::current = co_;
    co_->c1_ = std::move(co_->c1_).resume();
}

std::shared_ptr<coroutine> coroutine::current;