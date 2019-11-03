#pragma once
#include "Request.h"

namespace http {

/* http请求结构
|method     |space|URL  |space       |version|CRLF(\r\n)|
|header name|  :  |space|header value|CRLF   |
...headers
|CRLF       |
|requestbody|
*/
// 简单的有限状态机实现parser
class RequestParser
{
public:
    RequestParser(): state_(State::METHOD_START) {}
    ~RequestParser(){}

    enum ParseResult {
        GOOD,// 解析完成，格式上无误
        BAD,// 解析中发现请求格式错误
        UNFINISHED// 未解析完，等待后续字符流继续输入
    };

    ParseResult Parse(char *begin, char *end, Request &req);

private:
    enum State {
        METHOD_START,// 初始状态
        METHOD,
        URI,
        VERSION,
        CRLF_1,// 版本号后的换行意味着后面是header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CRLF_2,// 这里不同，可能是结束点
        EXPECT_END,
        END_HEADER,
        BODY,
        END
    } state_;

    std::string method_;
    std::string version_;

    std::string header_name_;
    std::string header_value_;

    std::string body_;

    ParseResult parse_one(char in, Request &req);
};

}// namespace http