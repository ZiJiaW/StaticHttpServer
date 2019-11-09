#include "pch.h"
#include "Response.h"


namespace http {

std::string Response::ToString()
{
    std::string res(headline_);
    for (auto header : headers_) {
        res.append(header.first);
        res += ": ";
        res.append(header.second);
        res += "\r\n";
    }
    res += "\r\n";
    res.append(body_);
    return res;
}

}// namespace http
