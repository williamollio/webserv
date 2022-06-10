//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTHEADER_HPP
#define WEBSERV_HTTHEADER_HPP


class HTTPHeader {
	public:
	    HTTPHeader();
	    std::string tostring() const;
		void set_content_type(std::string str);
		void set_content_length(int size);

	private :
		std::string	_status_message;
		int			_status_code;
		std::string _connection;
		std::string _content_encoding;
		std::string _content_type;
		int			_content_length;
		std::string _transfer_encoding;

};


#endif
