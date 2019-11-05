#pragma once

namespace http {

class Response
{
public:
    Response() = default;
    ~Response() = default;

    // 因为单次http应答发送完就会销毁，这里禁止copy，以减少不必要的开销
    // 也可以用智能指针，但是这样比较cool~~
    Response(const Response &rsp) = delete;
    Response operator=(const Response &rsp) = delete;

    Response(Response &&rsp) noexcept;
    Response operator=(Response &&rsp) noexcept;

private:
    std::vector<std::pair<std::string, std::string>> headers_;
    std::string content_;
    std::string body_;
};

}// namespace http

