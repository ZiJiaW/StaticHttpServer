#pragma once
#include "pch.h"
#include "RequestParser.h"
#include "Response.h"

namespace http {

class RequestHandler
{
public:
    RequestHandler(std::string root = ".") : root_path(root) {};
    ~RequestHandler() = default;

    // Handle a bad request.
    std::string HandleBadRequest(StatusCode code);
    // Handle a "good" request, which may be also bad after further analysis.
    void HandleGoodRequest(Request &req, std::function<void(const std::string &)> f);

private:
    std::string root_path;
    // this function decode uri to the targeted file path
    std::string decode(const std::string &uri);
    std::set<std::ifstream> fss;
};

}
