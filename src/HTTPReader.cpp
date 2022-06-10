//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPReader.hpp"
#include "HTTPRequest.hpp"
#include "HTTPRequestGet.hpp"
#include "CGIResponse.hpp"
#include "CGIResponseGet.hpp"
#include "CGIResponsePost.hpp"
#include "CGIResponseDelete.hpp"

HTTPReader::HTTPReader(): _socket() {}

HTTPReader::HTTPReader(Socket &socket): _socket(socket) {}

HTTPReader::~HTTPReader() {
    try {
        _socket.close_socket();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

void HTTPReader::run() {
    try {
        HTTPRequest request = _parse();
        CGIResponse * response;
        switch (request.getType()) {
            case HTTPRequest::GET:    response = new CGIResponseGet(request);    break;
            case HTTPRequest::POST:   response = new CGIResponsePost(request);   break;
            case HTTPRequest::DELETE: response = new CGIResponseDelete(request); break;
        }
        response->run(_socket);
        delete response;
    } catch (std::exception & ex) {
        // TODO: Error
//        sendError(ex.getErrorCode());
    }
}

HTTPRequest HTTPReader::_parse() throw(std::exception) {
    return HTTPRequestGet();
}
