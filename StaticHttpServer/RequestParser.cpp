#include "pch.h"
#include "RequestParser.h"

namespace http {

RequestParser::ParseResult RequestParser::Parse(char *begin, char *end, Request &req)
{
    while (begin != end) {
        auto r = parse_one(*begin++, req);
        if (r == ParseResult::GOOD || r == ParseResult::BAD)
            return r;
    }
    return ParseResult::UNFINISHED;
}


RequestParser::ParseResult RequestParser::parse_one(char in, Request &req)
{
    switch (state_)
    {
    case State::METHOD_START:
        method_.push_back(in);
        state_ = State::METHOD;
        return ParseResult::UNFINISHED;
    case State::METHOD:
        if (in == ' ') {
            state_ = State::URI;
            if (method_ == "GET") {
                req.set_method(Method::GET);
            }
            else if (method_ == "POST") {
                req.set_method(Method::POST);
            }
            else {
                return ParseResult::BAD;
            }
        }
        else method_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::URI:
        if (in == 'in')
            state_ = State::VERSION;
        else
            req.push_uri(in);
        return ParseResult::UNFINISHED;
    case State::VERSION:
        if (in == '\r') {
            state_ = State::CLRF_1;
            if (version_ != "HTTP/1.1")
                return ParseResult::BAD;
            req.set_version(Version::HTTP_1_1);
        }
        else
            version_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::CLRF_1:
        if (in == '\n')
            state_ = State::HEADER;
        else
            return ParseResult::BAD;
        return ParseResult::UNFINISHED;
    case State::HEADER:
        break;
    case State::HEADER_COLON:
        break;
    case State::HEADER_VALUE:
        break;
    case State::CLRF_2:
        break;
    case State::EXPECT_END:
        break;
    case State::END_HEADER:
        break;
    case State::BODY:
        break;
    case State::END:
        break;
    default:
        break;
    }
    return ParseResult::UNFINISHED;
}

}// namespace http
