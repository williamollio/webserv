#include "CGIResponseRedirect.hpp"
#include "CGIResponse.hpp"
#include "Connection.hpp"

CGIResponseRedirect::CGIResponseRedirect(HTTPRequest* request, Socket &socket, Runnable &runnable,
                                         const std::string &redirect) : CGIResponse(request, socket, runnable),
										 _counter(0),
                                         _redirect(redirect) {}


void CGIResponseRedirect::run() {
    HTTPHeader header;
	const std::string body = "Moved Permanently";
    header.setStatusCode(301);
    header.setStatusMessage(get_message(301));
	header.set_content_length(static_cast<int>(body.size()));
	header.set_content_type("text/plain");
    _payload = header.tostring()
               + "\r\nLocation: http://"
               + _request->getServerName()
			   + ":" + int_to_string(_request->getUsedPort())
               + _redirect + _request->getPath()
			   + "\r\n\r\n" + body;
	std::cout << "_payload redirect : " << _payload << std::endl;
	Connection::getInstance().add_fd(_socket.get_fd(), this, false);
}

bool CGIResponseRedirect::runForFD(int, bool hup) {
    if (hup) {
        _socket.close();
        return true;
    }
    try {
        ssize_t ret = _socket.write(_payload.c_str() + _counter, _payload.size() - _counter < 65536 ? _payload.size() - _counter : 65536);
        _counter += ret;
        if (_counter < _payload.size()) {
            return false;
        }
        debug("Write with socket fd " << _socket.get_fd() << " size " << _counter << " real " << _payload.size());
        debug("Closing socket fd " << _socket.get_fd());
        _socket.close();
        return true;
    } catch (IOException &) {
        debug("Write with socket fd " << _socket.get_fd() << " size " << _counter);
        return false;
    }
}