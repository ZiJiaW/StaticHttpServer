#pragma once
#include "Request.h"

namespace http {

/* http请求结构
|method|space|URL|space|version|CLRF(\r\n)|
|header name|:|space|header value|CLRF|
...headers
|CLRF|
|request body|
*/
// 通过有限状态机解析请求
class RequestParser
{
public:
    RequestParser(): state_(state::METHOD_START) {}
    ~RequestParser(){}

    enum parse_result {
        GOOD,// 解析完成，格式上无误
        BAD,// 解析中发现请求格式错误
        UNFINISHED// 未解析完，等待后续字符流继续输入
    };

    parse_result Parse(char *begin, char *end, Request &req);

private:
    enum state {
        METHOD_START,// 初始状态
        METHOD,
        URI,
        VERSION,
        CLRF_1,// 版本号后的换行意味着后面是header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CLRF_2,// 这里不同，可能是结束点
        EXPECT_END,
        END_HEADER,
        BODY,
        END
    } state_;

    parse_result parse_one(char in, Request &req);
};

}// namespace http