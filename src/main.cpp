#include "Connection.hpp"
#include "Socket.hpp"

int main() {
    signal(SIGPIPE, SIG_IGN);
	try
	{
		Connection	server_connection;
		server_connection.establishConnection();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
