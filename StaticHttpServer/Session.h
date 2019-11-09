#pragma once
#include "pch.h"
#include "Request.h"
#include "RequestHandler.h"
#include "Response.h"

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
        RequestHandler &request_handler);
    ~Session(){}

    void Open();
    void Close();

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
    // 临时存储response
    Response response_;
    // 请求处理句柄
    RequestHandler &request_handler_;
    // 每个会话分配一个parser，如果线程级公用的话会发生污染
    RequestParser parser_;
    // should close?
    bool to_close_;

    // R/W function
    void do_read();
    void do_write();
};

}// namespace http

