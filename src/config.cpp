#include "config.h"

config& config::get() {
	static config cfg;
	return cfg;
}

bool config::init(int argc, char* argv[]) {
	// 命令行
	// TODO 提供配置文件功能
	boost::program_options::options_description desc("数据流 event-source 转接服务：");
	std::string httpAddr = "127.0.0.1:8686";

	desc.add_options()
			("help,h", "帮助信息")
			("http", boost::program_options::value<std::string>(&httpAddr)->default_value("127.0.0.1:8686"), "HTTP EventSource 服务监听端口")
            ("node", boost::program_options::value<std::string>(&node)->default_value("node-1"), "节点名称 HTTP-Header X-Node: xxxxx 的值");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm); // 外部变量更新通知

	if(vm.count("help") > 0) {
		std::cerr << desc << std::endl;
		return false;
	}

	auto x = httpAddr.find_last_of(':');
	address.address(boost::asio::ip::make_address(httpAddr.substr(0, x)));
	std::uint16_t port = std::atoi(httpAddr.c_str() + x + 1);
	if(port == 0) {
		port = 8686;
	}
	address.port(port);
	return true;
}
