#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

class Server
{
public:
    // 不允许复制构造，默认构造也是删除的
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // 强制显式构造
    explicit Server(short port);
    // 调用点
    void Run();

    ~Server() {}

private:
    // 运行IO事件响应的载体
    boost::asio::io_context io_context_;
    // 监听端口上的TCP连接
    boost::asio::ip::tcp::acceptor acceptor_;
    // 用于处理程序异常退出，如强制被关闭进程
    boost::asio::signal_set signals_;
    // 管理所有http会话
    SessionController session_controller_;
    // 用于处理请求
    RequestHandler request_handler_;
    // 服务器运行的线程数
    int nr_threads_;
    // 发出接收请求
    void do_accept();
    // 结束时，graceful shutdown
    void do_shutdown();
};

}// namespace http