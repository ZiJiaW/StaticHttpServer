#include "pch.h"
#include "RequestParser.h"

namespace http {

RequestParser::ParseResult RequestParser::Parse(char *begin, char *end, Request &req)
{
    while (begin != end) {
        auto r = parse_one(*begin++, req);
        
        if (r == ParseResult::GOOD || r == ParseResult::BAD) {
            //std::cout << r << std::endl;
            return r;
        }
            
    }
    return ParseResult::UNFINISHED;
}

/* Implementation of FSM.
Note: this function doesn't fully check the correctness of request, it
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
        if (in == ' ')
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
        else {
            header_name_.clear();
            return ParseResult::BAD;
        }
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
                return ParseResult::GOOD;
            case Method::POST:
                if (req.content_length() == 0) {
                    return ParseResult::GOOD;
                }
                if (req.chunked()) {
                    state_ = State::CHUNK_LENGTH;
                    return ParseResult::UNFINISHED;
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
        return body_.size() > req.content_length() ? (body_.clear(), ParseResult::BAD) : ParseResult::UNFINISHED;
    case State::CHUNK_LENGTH:
        if (in == '\r') {
            state_ = State::AFTER_LENGTH;
            try {
                chunk_length_ = stoi(s_chunk_length_, 0, 16);
            }
            catch (std::exception e) {
                std::cout << e.what() << std::endl;
                s_chunk_length_.clear();
                chunk_length_ = 0;
                return ParseResult::BAD;
            }
            s_chunk_length_.clear();
            return ParseResult::UNFINISHED;
        }
        s_chunk_length_.push_back(in);
        return ParseResult::UNFINISHED;
    case State::AFTER_LENGTH:
        if (in == '\n') {
            state_ = chunk_length_ == 0 ? State::BEFORE_END : State::CHUNK_BODY;
            return ParseResult::UNFINISHED;
        }
        s_chunk_length_.clear();
        chunk_length_ = 0;
        return ParseResult::BAD;
    case State::CHUNK_BODY:
        if (chunk_length_ == 0) {
            state_ = State::AFTER_BODY;
            return in == '\r' ? ParseResult::UNFINISHED : (chunk_length_ = 0, ParseResult::BAD);
        }
        req.push_body(in);
        return --chunk_length_ < 0 ? (chunk_length_ = 0, ParseResult::BAD) : ParseResult::UNFINISHED;
    case State::AFTER_BODY:
        if (in == '\n') {
            state_ = State::CHUNK_LENGTH;
            return ParseResult::UNFINISHED;
        }
        return ParseResult::BAD;
    case State::BEFORE_END:
        chunk_length_ = 0;
        s_chunk_length_.clear();
        if (in == '\r') {
            state_ = State::END;
            return ParseResult::UNFINISHED;
        }
        return ParseResult::BAD;
    case State::END:
        return in == '\n' ? ParseResult::GOOD : ParseResult::BAD;
    default:
        break;
    }
    return ParseResult::BAD;
}

// 这里只需要将状态掰回原位
// 不需要做多余的清理，在状态机返回时已完成缓存的清理
void RequestParser::Reset()
{
    state_ = State::METHOD_START;
}

}// namespace http
