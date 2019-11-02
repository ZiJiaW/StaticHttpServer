#pragma once

namespace http {

// 当前仅支持两种请求方法
enum Method {
    GET, POST
};

// 支持版本1.1
enum Version {
    HTTP_1_1
};

class Request
{
public:
    Request(){}
    ~Request(){}

    inline void set_method(Method m) { method_ = m; }
    inline void set_version(Version v) { version_ = v; }
    inline void set_uri(std::string &&s) { uri_ = s; }
    inline void set_header(const std::string &name, const std::string &value) { headers_[name] = value; }
    inline void set_body(const std::string && s) { body_ = s; }


private:
    Method method_;
    Version version_;
    std::string uri_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;

    // some important value
    int content_length_;
    bool keep_alive_;
};

}// namespace http

