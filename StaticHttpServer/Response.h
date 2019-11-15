#pragma once
#include "basic.h"

namespace http {

class Response
{
public:
    Response() = default;
    ~Response() = default;

    // 因为单次http应答发送完就会销毁，这里禁止copy，以减少不必要的开销
    // 也可以用智能指针，但是这样比较cool~~
    Response(const Response &rsp) = delete;
    Response &operator=(const Response &rsp) = delete;

    Response(Response &&rsp) noexcept = default;
    Response &operator=(Response &&rsp) noexcept = default;

    void set_code(StatusCode code) { code_ = code; }
    void set_headline(const std::string &hl) { headline_ = hl; }
    void set_header(std::string &&name, std::string &&value) { headers_.push_back(std::make_pair(std::move(name), std::move(value))); }
    void set_body(std::string &&body) { body_ = std::move(body); }
    void push_body(const char *buf, std::size_t size) { body_.append(buf, size); }
    std::size_t body_size() { return body_.size(); }

    std::string ToString();
private:
    std::string headline_;
    std::vector<std::pair<std::string, std::string>> headers_;
    std::string body_;
    StatusCode code_;
};

}// namespace http

