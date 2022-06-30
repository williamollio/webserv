#include "Connection.hpp"
#include "Configuration.hpp"

int main(const int argc, const char **argv) {
    signal(SIGPIPE, SIG_IGN);
	try
	{
//		Configuration	config("server.conf");
		if (argc == 2)
			Configuration::getInstance().load_config_file(argv[1]);
		else
        	Configuration::getInstance().load_config_file();
		// std::cout << config;
		// std::cout << "config loaded!" << std::endl;
		Connection	server_connection;
		server_connection.establishConnection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}
