#pragma once
#include "RequestParser.h"

namespace http {

class RequestHandler
{
public:
    RequestHandler() = default;
    ~RequestHandler() = default;

    RequestParser &parser() { return request_parser_; }

    Response HandleBadRequest(StatusCode code);
    Response HandleGoodRequest(const Request &req);

private:
    // 用于解析请求的parser
    RequestParser request_parser_;

};

}
