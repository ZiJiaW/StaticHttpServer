#pragma once
#include "SessionController.h"
#include "Request.h"
#include "pch.h"
#include "RequestHandler.h"

namespace http {

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(const Session &session) = delete;
    Session operator=(const Session &session) = delete;
    explicit Session(boost::asio::ip::tcp::socket socket,
        boost::asio::io_context &io_context,
        SessionController &session_controller,
        RequestHandler &request_handler);
    ~Session(){}

    void Run();

private:
    // ���λỰ��socket����
    boost::asio::ip::tcp::socket socket_;
    // ��˴����Ӱ󶨵�IO Context
    boost::asio::io_context &io_context_;
    // ȫ��Ψһ�Ŀ��ƶ���
    SessionController &session_controller_;
    // ����8KB��С�Ľ��ջ�����
    std::array<char, 8192> buffer_;
    // һ�������������request
    Request request_;
    // ��������
    RequestHandler &request_handler_;
};

}// namespace http

