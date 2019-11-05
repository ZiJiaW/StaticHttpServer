#pragma once
#include "Request.h"

namespace http {

/* http����ṹ
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
// ����http������﷨����
class RequestParser
{
public:
    RequestParser(): state_(State::METHOD_START) {}
    ~RequestParser() = default;

    enum ParseResult {
        GOOD,// ������ɣ���ʽ������
        BAD,// �����з��������ʽ����
        UNFINISHED// δ�����꣬�ȴ������ַ�����������
    };

    ParseResult Parse(char *begin, char *end, Request &req);

    void Reset();

private:
    // ͨ������״̬��ģ����ʵ��parser
    // ����������״̬�Ķ���
    enum State {
        METHOD_START,// ��ʼ״̬
        METHOD,
        URI,
        VERSION,
        CRLF_1,// �汾�ź�Ļ�����ζ�ź�����header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CRLF_2,// ���ﲻͬ�������ǽ����㣬û��body
        EXPECT_END,
        END_HEADER,
        BODY,// ��ͨ�ķǷֿ�����
        CHUNK_LENGTH,// ���ﴦ����ܵķֿ�����
        AFTER_LENGTH,
        CHUNK_BODY,
        AFTER_BODY,
        BEFORE_END,
        END// �ֿ�����Ľ�����
    } state_;

    // ������Ӧ���ַ���
    std::string method_;
    std::string version_;

    std::string header_name_;
    std::string header_value_;

    std::string body_;
    std::string s_chunk_length_;

    int chunk_length_;

    // ���ַ�����״̬ת��
    ParseResult parse_one(char in, Request &req);
};

}// namespace http