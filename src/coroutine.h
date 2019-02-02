#pragma once
#include "vendor.h"


class coroutine;
class coroutine_handler {
public:
    coroutine_handler();
    coroutine_handler(std::shared_ptr<coroutine> co);
    coroutine_handler(const coroutine_handler& ch) = default;
    coroutine_handler& operator[](boost::system::error_code& error);
    inline void operator()(const boost::system::error_code& error, std::size_t size = 0) {
        resume(error, size);
    }
    void reset();
    void reset(std::shared_ptr<coroutine> co);

    void yield();
    void yield(boost::system::error_code& error);
    void resume();
    void resume(const boost::system::error_code& error, std::size_t size = 0);
public:
    std::size_t size_;
private:
    std::shared_ptr<coroutine> co_;
    std::shared_ptr<boost::system::error_code> error_;
};

class coroutine {
public:
    static std::shared_ptr<coroutine> current;
    template <class Executor, class Handler>
    static void start(Executor& et, Handler&& fn) {
        auto co = std::make_shared<coroutine>();
        coroutine_handler ch(co);
        auto guard = boost::asio::make_work_guard(et);
        boost::asio::post(et, [co, ch, fn, guard] () mutable {
            co->c1_ = boost::context::fiber([co, ch, fn, guard] (boost::context::fiber&& c2) mutable {
                co->c2_ = std::move(c2);

                fn(ch);
                guard.reset();
                return std::move(co->c2_);
            });
            ch.resume();
        });
    }
private:
    boost::context::fiber c1_;
    boost::context::fiber c2_;
    friend class coroutine_handler;
};

namespace boost::asio {
    template <>
    class async_result<::coroutine_handler> {
    public:
        explicit async_result(::coroutine_handler& ch)
        : ch_(ch) {}
        using type = std::size_t;
        type get() {
            ch_.yield();
            return ch_.size_;
        }
    private:
        ::coroutine_handler& ch_;
    };
}