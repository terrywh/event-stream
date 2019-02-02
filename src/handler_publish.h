#pragma once
#include "vendor.h"
#include "handler.h"

class handler_publish: public handler {
public:
    explicit handler_publish(handler&& h);
    virtual void run(coroutine_handler yield) override;
};
