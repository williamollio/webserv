//
// Created by Manuel Hahn on 6/10/22.
//

#include "../include/CGIResponseGet.hpp"
#include "HTTPHeader.hpp"
#include <fstream>
#include <algorithm>

std::string CGIResponseGet::set_file(std::string path)
{
	std::string tmp;

	if (path == "/")
	{
		_file_extension = "html";
		tmp = "index.html";
	}
	else
	{
		tmp = path.erase(0,1);
		size_t pos = tmp.find('.', 0);
		if (pos == std::string::npos)
			_file_extension = "";
		else
			_file_extension = tmp.substr(pos);
	}
	return (tmp);
}

std::string CGIResponseGet::construct_content_type()
{
	std::string tmp;

	tmp = "text/";
	if (_file_extension == "html")
		tmp += _file_extension;
	else
		tmp += "plain";
	return (tmp);
}

void CGIResponseGet::run(Socket & socket) {
	std::string body;

	HTTPHeader header;
	std::string file = set_file(_request._path);
	body = read_file(file);
	header.set_content_type(construct_content_type());
	header.set_content_length(body.size());
    header.setStatusCode(200);
    header.setStatusMessage("OK");
    std::cout << "header sent back : " << header.tostring() << std::endl;
	socket.send(header.tostring() + "\n\n" + body);
}

CGIResponseGet::CGIResponseGet(HTTPRequest &request) : CGIResponse(request) {}
