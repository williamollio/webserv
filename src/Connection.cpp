#include "Connection.hpp"

Connection::Connection() {}
Connection::~Connection() {}

void Connection::establishConnection() {
	_connection_one = new Socket();
}