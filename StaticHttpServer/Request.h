#pragma once
#include "basic.h"

namespace http {

class Request
{
public:
    Request(): is_valid_(true), keep_alive_(true), status_(StatusCode::OK), chunked_(false) {}
    ~Request() = default;

    void set_method(std::string &&m);
    void set_version(std::string &&v);
    void push_uri(char ch) { uri_.push_back(ch); }
    void set_body(std::string &&s) { body_ = s; }
    void push_body(char ch) { body_.push_back(ch); }
	const std::string get_body() { return body_; }
    Method method() { return method_; }

    bool ok() const { return is_valid_; }

    void set_header(std::string &&name, std::string &&value);
    StatusCode status() { return status_; }

    int content_length() { return content_length_; }
    bool chunked() { return chunked_; }
    bool keep_alive() { return keep_alive_; }

    const std::string &get(const std::string &name) const { return headers_.at(name); }
    const std::string &uri() const { return uri_; }
	
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

