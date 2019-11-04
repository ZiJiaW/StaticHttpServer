#pragma once

namespace http {

class Response
{
public:
    Response(){}
    ~Response(){}

private:
    std::vector<std::pair<std::string, std::string>> headers_;
    std::string content_;

};

}// namespace http

