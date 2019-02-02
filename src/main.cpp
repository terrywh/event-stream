#include "vendor.h"
#include "config.h"
#include "coroutine.h"
#include "server.h"

int main(int argc, char* argv[]) {
    if(!config::get().init(argc, argv)) {
    	return -1;
    }
    server ss;
    coroutine::start(config::get().context, std::bind(&server::run, &ss, std::placeholders::_1));
	
    config::get().context.run();
    return 0;
}
