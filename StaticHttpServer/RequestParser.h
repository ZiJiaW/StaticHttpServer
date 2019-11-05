#pragma once
#include "Request.h"

namespace http {

/* http请求结构
|method     |space|URL  |space       |version|CRLF(\r\n)|
|header name|  :  |space|header value|CRLF   |
...headers
|CRLF       |
|requestbody|

body may be chunked:
|hex length|CRLF|
|chunk1    |CRLF|
|hex length|CRLF|
|chunk2    |CRLF|
...
|0|CRLF|
|CRLF|
*/
// 处理http请求的语法分析
class RequestParser
{
public:
    RequestParser(): state_(State::METHOD_START) {}
    ~RequestParser() = default;

    enum ParseResult {
        GOOD,// 解析完成，格式上无误
        BAD,// 解析中发现请求格式错误
        UNFINISHED// 未解析完，等待后续字符流继续输入
    };

    ParseResult Parse(char *begin, char *end, Request &req);

    void Reset();

private:
    // 通过有限状态机模型来实现parser
    // 这里是所有状态的定义
    enum State {
        METHOD_START,// 初始状态
        METHOD,
        URI,
        VERSION,
        CRLF_1,// 版本号后的换行意味着后面是header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CRLF_2,// 这里不同，可能是结束点，没有body
        EXPECT_END,
        END_HEADER,
        BODY,// 普通的非分块请求
        CHUNK_LENGTH,// 这里处理可能的分块请求
        AFTER_LENGTH,
        CHUNK_BODY,
        AFTER_BODY,
        BEFORE_END,
        END// 分块请求的结束点
    } state_;

    // 缓存响应的字符串
    std::string method_;
    std::string version_;

    std::string header_name_;
    std::string header_value_;

    std::string body_;
    std::string s_chunk_length_;

    int chunk_length_;

    // 按字符进行状态转移
    ParseResult parse_one(char in, Request &req);
};

}// namespace http