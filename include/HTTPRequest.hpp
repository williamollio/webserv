//
// Created by Manuel Hahn on 6/10/22.
//

#ifndef WEBSERV_HTTPREQUEST_HPP
#define WEBSERV_HTTPREQUEST_HPP


class HTTPRequest {
public:
    enum TYPE {
        GET, POST, DELETE
    };

    TYPE getType() const;

protected:
    explicit HTTPRequest(TYPE);

private:
    const TYPE _type;
};


#endif //WEBSERV_HTTPREQUEST_HPP
