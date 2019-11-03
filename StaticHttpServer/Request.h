#pragma once
#include "basic.h"

namespace http {

class Request
{
public:
    Request(): is_valid_(true), keep_alive_(true), status_(StatusCode::OK) {}
    ~Request(){}

    inline void set_method(std::string &&m);
    inline void set_version(std::string &&v);
    inline void push_uri(char ch) { uri_.push_back(ch); }
    inline void set_body(const std::string && s) { body_ = s; }

    inline Method method() { return method_; }

    inline bool ok() { return is_valid_; }

    void set_header(std::string &&name, std::string &&value);

    inline int content_length() { return content_length_; }
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

    // is valid?
    bool is_valid_;
};

}// namespace http

