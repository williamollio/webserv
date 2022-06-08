//
// Created by Manuel Hahn on 6/8/22.
//

#include "HTTPReader.hpp"

HTTPReader::HTTPReader(): _socket() {}

HTTPReader::HTTPReader(Socket &socket): _socket(socket) {}

HTTPReader::~HTTPReader() {
    try {
        _socket.close_socket();
    } catch (std::exception &exception) {
        std::cerr << "HERE" << exception.what() << std::endl;
    }
}

void HTTPReader::run() {
    _socket.read_socket();
    _socket.send_header("html");
    _socket.send_file("index.html");
}
