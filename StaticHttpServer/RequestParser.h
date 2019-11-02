#pragma once
#include "Request.h"

namespace http {

/* http����ṹ
|method|space|URL|space|version|CLRF(\r\n)|
|header name|:|space|header value|CLRF|
...headers
|CLRF|
|request body|
*/
// ͨ������״̬����������
class RequestParser
{
public:
    RequestParser(): state_(state::METHOD_START) {}
    ~RequestParser(){}

    enum parse_result {
        GOOD,// ������ɣ���ʽ������
        BAD,// �����з��������ʽ����
        UNFINISHED// δ�����꣬�ȴ������ַ�����������
    };

    parse_result Parse(char *begin, char *end, Request &req);

private:
    enum state {
        METHOD_START,// ��ʼ״̬
        METHOD,
        URI,
        VERSION,
        CLRF_1,// �汾�ź�Ļ�����ζ�ź�����header
        HEADER,
        HEADER_COLON,
        HEADER_VALUE,
        CLRF_2,// ���ﲻͬ�������ǽ�����
        EXPECT_END,
        END_HEADER,
        BODY,
        END
    } state_;

    parse_result parse_one(char in, Request &req);
};

}// namespace http