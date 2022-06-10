//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTHEADER_HPP
#define WEBSERV_HTTHEADER_HPP

#include <string>

class HTTPHeader {
	public:
	    HTTPHeader();
	    std::string tostring() const;

        const std::string &getContentType() const;
        int getContentLength() const;
        const std::string &getTransferEncoding() const;
        const std::string &getStatusMessage() const;
        int getStatusCode() const;
        const std::string &getConnection() const;
        const std::string &getContentEncoding() const;
        void set_content_type(const std::string & str);
		void set_content_length(int size);
        void setTransferEncoding(const std::string &transferEncoding);
        void setStatusMessage(const std::string &statusMessage);
        void setStatusCode(int statusCode);
        void setConnection(const std::string &connection);
        void setContentEncoding(const std::string &contentEncoding);

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
