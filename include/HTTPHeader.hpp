//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTPHEADER_HPP
#define WEBSERV_HTTPHEADER_HPP

#include <string>

class HTTPHeader {
	public:
	    HTTPHeader();

	    std::string         tostring()            const;
        const std::string & getContentType()      const;
        int                 getContentLength()    const;
        const std::string & getTransferEncoding() const;
        const std::string & getStatusMessage()    const;
        int                 getStatusCode()       const;
        const std::string & getConnection()       const;
        const std::string & getContentEncoding()  const;
        void                set_content_type(const std::string &);
		void                set_content_length(int);
        void                setTransferEncoding(const std::string &);
        void                setStatusMessage(const std::string &);
        void                setStatusCode(int);
        void                setConnection(const std::string &);
        void                setContentEncoding(const std::string &);

private:
		std::string	_status_message;
        int			_status_code;
		std::string _connection;
		std::string _content_encoding;
		std::string _content_type;
		int			_content_length;
		std::string _transfer_encoding;

};

#endif /* WEBSERV_HTTPHEADER_HPP */
