#pragma once
#include "pch.h"
#include "Request.h"
#include "RequestHandler.h"

namespace http {

class SessionController;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(const Session &session) = delete;
    Session &operator=(const Session &session) = delete;
    explicit Session(boost::asio::ip::tcp::socket socket,
        boost::asio::io_context &io_context,
        SessionController &session_controller,
        RequestHandler &request_handler,
        boost::asio::ssl::context &ssl_context);
    ~Session(){}

    void Open();
    void Close();

private:
    // ���λỰ��socket����(ssl֧��)
    //boost::asio::ip::tcp::socket socket_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    // ��˴����Ӱ󶨵�IO Context
    boost::asio::io_context &io_context_;
    // ȫ��Ψһ�Ŀ��ƶ���
    SessionController &session_controller_;
    // ����8KB��С�Ľ��ջ�����
    std::array<char, 8192> buffer_;
    // һ�������������request
    Request request_;
    // ��ʱ�洢response
    std::string response_;
    // ��������
    RequestHandler &request_handler_;
    // ÿ���Ự����һ��parser������̼߳����õĻ��ᷢ����Ⱦ
    RequestParser parser_;
    // should close?
    bool to_close_;

    // R/W function
    void do_read();
    void do_write();
    // SSL handshake
    void do_handshake();
};

}// namespace http

