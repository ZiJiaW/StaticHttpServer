#pragma once
#include "basic.h"

namespace http {

class Response
{
public:
    Response() = default;
    ~Response() = default;

    // ��Ϊ����httpӦ������ͻ����٣������ֹcopy���Լ��ٲ���Ҫ�Ŀ���
    // Ҳ����������ָ�룬���������Ƚ�cool~~
    Response(const Response &rsp) = delete;
    Response &operator=(const Response &rsp) = delete;

    Response(Response &&rsp) noexcept = default;
    Response &operator=(Response &&rsp) noexcept = default;

    void set_code(StatusCode code) { code_ = code; }


    std::string ToString();
private:
    std::vector<std::pair<std::string, std::string>> headers_;
    std::string content_;
    std::string body_;
    StatusCode code_;
};

}// namespace http

