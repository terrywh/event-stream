#pragma once
#include "vendor.h"
#include "http_handler.h"

class http_handler_publish: public http_handler {
public:
    explicit http_handler_publish(http_handler&& h);
    virtual void run(coroutine_handler yield) override;
};
