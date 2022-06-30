#include "Connection.hpp"
#include "Configuration.hpp"

int main(const int argc, const char ** argv) {
    if (argc != 2) {
        std::clog << "Missing configuration file!"  << std::endl
                  << "Usage: webserv <config file>" << std::endl;
        return EXIT_FAILURE;
    }
    signal(SIGPIPE, SIG_IGN);
	try
	{
        Configuration::getInstance().load_config_file(argv[1]);
		Connection server_connection;
		server_connection.establishConnection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}
