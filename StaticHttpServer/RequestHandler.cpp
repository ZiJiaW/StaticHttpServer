#include "pch.h"
#include "RequestHandler.h"

namespace http {

std::unordered_map<StatusCode, std::string> RequestHandler::response_str_map = {
    {StatusCode::OK, "HTTP/1.1 200 OK\r\n"},
    {StatusCode::CREATED, "HTTP/1.1 201 Created\r\n"},
    {StatusCode::ACCEPTED, "HTTP/1.1 202 Accepted\r\n"},
    {StatusCode::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\r\n"},
    {StatusCode::BAD_REQUEST, "HTTP/1.1 400 Bad Request\r\n"},
    {StatusCode::FORBIDDEN, "HTTP/1.1 403 Forbidden\r\n"},
    {StatusCode::INTERNAL_ERROR, "HTTP/1.1 500 Internal Server Error\r\n"},
    {StatusCode::LENGTH_REQUIRED, "HTTP/1.1 411 Length Required\r\n"},
    {StatusCode::METHOD_NOT_ALLOWED, "HTTP/1.1 405 Method Not Allowed\r\n"},
    {StatusCode::NOT_ACCEPTABLE, "HTTP/1.1 406 Not Acceptable\r\n"},
    {StatusCode::NOT_FOUND, "HTTP/1.1 404 Not Found\r\n"},
    {StatusCode::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\r\n"},
    {StatusCode::NO_CONTENT, "HTTP/1.1 204 No Content\r\n"},
    {StatusCode::REQUEST_TIMEOUT, "HTTP/1.1 408 Request Timeout\r\n"},
    {StatusCode::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service Unavailable\r\n"},
    {StatusCode::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\r\n"},
    {StatusCode::VERSION_NOT_SUPPORTED, "HTTP/1.1 505 Version Not Supported\r\n"},
};

std::string RequestHandler::HandleBadRequest(StatusCode code)
{
    Response rs;
    rs.set_headline(response_str_map[code]);
    std::string body = "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>"+std::to_string(static_cast<int>(code))+" Error</h1></body>"
        "</html>";
    rs.set_header("Content-Length", std::to_string(body.size()));
    rs.set_header("Content-Type", "text/html");
    rs.set_header("Server", "SHS");
    rs.set_body(std::move(body));
    return rs.ToString();
}

std::string RequestHandler::HandleGoodRequest(const Request &req)
{
    // test
    return HandleBadRequest(StatusCode::NOT_FOUND);
}


}// namespace http
