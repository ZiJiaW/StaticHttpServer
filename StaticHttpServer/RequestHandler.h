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
    // Handle a Get request, which may be also bad after further analysis.
    void HandleGetRequest(Request &req, std::function<void(const std::string &)> f);

    // Handle Post
    std::string HandlePostRequest(Request &req);
    

private:
    std::string root_path;
    // this function decode uri to the targeted file path
    std::string decode(const std::string &uri);
    std::set<std::ifstream> fss;

    void PostResponseOK(Response &rs);
    std::size_t find_next(const std::string &str, const std::string &aim, std::size_t pos);
};

}
