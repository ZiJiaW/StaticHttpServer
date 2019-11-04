#pragma once
#include "basic.h"

namespace http {

class Request
{
public:
    Request(): is_valid_(true), keep_alive_(true), status_(StatusCode::OK), chunked_(false) {}
    ~Request(){}

    void set_method(std::string &&m);
    void set_version(std::string &&v);
    void push_uri(char ch) { uri_.push_back(ch); }
    void set_body(std::string &&s) { body_ = s; }
    void push_body(char ch) { body_.push_back(ch); }

    Method method() { return method_; }

    bool ok() { return is_valid_; }

    void set_header(std::string &&name, std::string &&value);

    int content_length() { return content_length_; }
    bool chunked() { return chunked_; }
private:
    Method method_;
    Version version_;
    std::string uri_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;

    // some important value
    int content_length_;
    bool keep_alive_;
    StatusCode status_;
    bool chunked_;

    // is valid?
    bool is_valid_;
};

}// namespace http

