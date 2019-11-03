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

/* Implementation of FSM.
Note: this function doesn't check the correctness of request, it
      just segments the request stream.
*/
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
            req.set_method(std::move(method_));
        }
        else
            method_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::URI:
        if (in == 'in')
            state_ = State::VERSION;
        else
            req.push_uri(in);
        return ParseResult::UNFINISHED;
    case State::VERSION:
        if (in == '\r') {
            state_ = State::CRLF_1;
            req.set_version(std::move(version_));
        }
        else
            version_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::CRLF_1:
        if (in == '\n')
            state_ = State::HEADER;
        else
            return ParseResult::BAD;
        return ParseResult::UNFINISHED;
    case State::HEADER:
        if (in == ':')
            state_ = State::HEADER_COLON;
        else
            header_name_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::HEADER_COLON:
        if (in == ' ')
            state_ = State::HEADER_VALUE;
        else
            return ParseResult::BAD;
        return ParseResult::UNFINISHED;
    case State::HEADER_VALUE:
        if (in == '\r') {
            state_ = State::CRLF_2;
            req.set_header(std::move(header_name_), std::move(header_value_));
        }
        else
            header_value_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::CRLF_2:
        if (in == '\n')
            state_ = State::EXPECT_END;
        else
            return ParseResult::BAD;
        return ParseResult::UNFINISHED;
    case State::EXPECT_END:
        if (in == '\r')
            state_ = State::END_HEADER;
        else {
            header_name_.push_back(in);
            state_ = State::HEADER;
        }
        return ParseResult::UNFINISHED;
    case State::END_HEADER:
        if (in == '\n') {
            switch (req.method())
            {
            case Method::GET:
                state_ = State::END;
                return ParseResult::GOOD;
            case Method::POST:
                if (req.content_length() == 0) {
                    state_ = State::END;
                    return ParseResult::GOOD;
                }
                state_ = State::BODY;
                return ParseResult::UNFINISHED;
            default:// this won't happen
                return ParseResult::BAD;
            }
        }
        else
            return ParseResult::BAD;
    case State::BODY:
        body_.push_back(in);
        if (body_.size() == req.content_length()) {
            req.set_body(std::move(body_));
            return ParseResult::GOOD;
        }
        return ParseResult::UNFINISHED;
    case State::END:
        break;
    default:
        break;
    }
    return ParseResult::BAD;
}

}// namespace http
