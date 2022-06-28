#include "Connection.hpp"
#include "Socket.hpp"
#include "Configuration.hpp"

int main() {
    signal(SIGPIPE, SIG_IGN);
	try
	{
		Configuration	config("server.conf");
		Connection	server_connection;
		server_connection.establishConnection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}
