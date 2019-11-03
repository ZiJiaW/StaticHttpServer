#include "pch.h"
#include "Request.h"

namespace http {

inline void Request::set_version(std::string &&v)
{
    if (v != "HTTP/1.1") {
        is_valid_ = false;
        status_ = StatusCode::VERSION_NOT_SUPPORTED;
    }
    else
        version_ = Version::HTTP_1_1;
    v.clear();
}

inline void Request::set_method(std::string &&m)
{
    if (m == "GET")
        method_ = Method::GET;
    else if (m == "POST")
        method_ = Method::POST;
    else {
        is_valid_ = false;
        status_ = StatusCode::METHOD_NOT_ALLOWED;
    }
    m.clear();
}

void Request::set_header(std::string &&name, std::string &&value)
{
    if (name == "Connection") {
        keep_alive_ = value != "close";// HTTP/1.1默认采取长连接
    }
    if (name == "Content-Length") {// 对于POST请求记录段长
        try {
            content_length_ = std::stoi(value);
        }
        catch (const std::invalid_argument &ex) {
            status_ = StatusCode::BAD_REQUEST;
            is_valid_ = false;
        }
        catch (const std::out_of_range &ex) {
            status_ = StatusCode::BAD_REQUEST;
            is_valid_ = false;
        }
    }
    // TODO: some other headers to analyze
    // ...
    headers_.emplace(std::make_pair(name, value));
}

}// namespace http