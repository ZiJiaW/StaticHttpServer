#pragma once
#include "RequestParser.h"

namespace http {

class RequestHandler
{
public:
    RequestHandler(){}
    ~RequestHandler(){}

    RequestParser &parser() { return request_parser_; }

private:
    // ���ڽ��������parser
    RequestParser request_parser_;

};

}
