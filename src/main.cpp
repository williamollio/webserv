#include "Connection.hpp"
#include "Socket.hpp"
#include "Configuration.hpp"

int main() {
	try
	{
		Connection	server_connection;
		Configuration	config("server.conf");
		std::cout << config;
		std::cout << "config loaded!" << std::endl;
		server_connection.establishConnection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}


}