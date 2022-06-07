#pragma once
#include "Socket.hpp"

class Connection {
private:
	Socket*	_connection_one;
public:
	Connection();
	~Connection();
	void	establishConnection();
protected:
};