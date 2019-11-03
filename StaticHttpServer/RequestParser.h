#pragma once
#include "Request.h"

namespace http {

/* http����ṹ
|method     |space|URL  |space       |version|CRLF(\r\n)|
|header name|  :  |space|header value|CRLF   |
...headers
|CRLF       |
|requestbody|
*/
// �򵥵�����״̬��ʵ��parser
class RequestParser
{
public:
    RequestParser(): state_(State::METHOD_START) {}
    ~RequestParser(){}

    enum ParseResult {
        GOOD,// ������ɣ���ʽ������
        BAD,// �����з��������ʽ����
        UNFINISHED// δ�����꣬�ȴ������ַ�����������
    };

    ParseResult Parse(char *begin, char *end, Request &req);

private:
    enum State {
        METHOD_START,// ��ʼ״̬
        METHOD,
        URI,
        VERSION,
        CRLF_1,// �汾�ź�Ļ�����ζ�ź�����header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CRLF_2,// ���ﲻͬ�������ǽ�����
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