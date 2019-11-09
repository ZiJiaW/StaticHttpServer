#pragma once
#include "pch.h"
#include "RequestParser.h"
#include "Response.h"

namespace http {

class RequestHandler
{
public:
    RequestHandler() = default;
    ~RequestHandler() = default;

    // Handle a bad request.
    Response HandleBadRequest(StatusCode code);
    // Handle a "good" request, which may be also bad after further analysis.
    Response HandleGoodRequest(const Request &req);

private:
    static std::unordered_map<StatusCode, std::string> response_str_map;

};

}
