#include "pch.h"
#include "RequestParser.h"

namespace http {

RequestParser::parse_result RequestParser::Parse(char *begin, char *end, Request &req)
{
    while (begin != end) {
        auto r = parse_one(*begin++, req);
        if (r == GOOD || r == BAD)
            return r;
    }
    return UNFINISHED;
}

RequestParser::parse_result RequestParser::parse_one(char in, Request &req)
{
    return UNFINISHED;
}

}// namespace http
