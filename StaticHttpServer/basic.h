#pragma once
#include "pch.h"

namespace http {

// only two methods now.
enum Method {
    GET, POST
};

// version 1.1
enum Version {
    HTTP_1_1
};

enum StatusCode {
    // 2XX
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    // don't support 3xx
    // 4XX
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    REQUEST_TIMEOUT = 408,
    LENGTH_REQUIRED = 411,
    // 5XX
    INTERNAL_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    VERSION_NOT_SUPPORTED = 505
};

namespace mime_types {

std::string GetType(const std::string &extension);

extern std::string default_type;
extern std::unordered_map<std::string, std::string> mime_map;

}// namespace mime_types

}// namespace http