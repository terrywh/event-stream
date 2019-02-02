#include "vendor.h"
#include "config.h"
#include "coroutine.h"
#include "http_server.h"

int main(int argc, char* argv[]) {
    if(!config::get().init(argc, argv)) {
    	return -1;
    }
    http_server ss;
    coroutine::start(config::get().context, std::bind(&http_server::run, &ss, std::placeholders::_1));

    config::get().context.run();
    return 0;
}
