#pragma once

namespace http {

class Response
{
public:
    Response() = default;
    ~Response() = default;

    // ��Ϊ����httpӦ������ͻ����٣������ֹcopy���Լ��ٲ���Ҫ�Ŀ���
    // Ҳ����������ָ�룬���������Ƚ�cool~~
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

