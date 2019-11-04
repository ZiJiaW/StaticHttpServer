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
    // 本次会话的socket对象
    boost::asio::ip::tcp::socket socket_;
    // 与此次连接绑定的IO Context
    boost::asio::io_context &io_context_;
    // 全局唯一的控制对象
    SessionController &session_controller_;
    // 设置8KB大小的接收缓冲区
    std::array<char, 8192> buffer_;
    // 一次请求解析出的request
    Request request_;
    // 请求处理句柄
    RequestHandler &request_handler_;
};

}// namespace http

