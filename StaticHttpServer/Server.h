#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

using IO_Ctx_Ptr = std::shared_ptr<boost::asio::io_context>;

class Server
{
public:
    // 不允许复制构造，默认构造也是删除的
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // 强制显式构造
    // 默认按照处理器线程数来创建线程池
    explicit Server(short port, int nr_threads = std::thread::hardware_concurrency());
    // 调用点
    void Run();

    ~Server() {}

private:
    // 线程数
    int nr_threads_;
    // 多个运行IO事件响应的载体
    std::vector<IO_Ctx_Ptr> io_contexts_;
    // 监听端口上的TCP连接
    boost::asio::ip::tcp::acceptor acceptor_;
    // 用于处理程序异常退出，如强制被关闭进程
    boost::asio::signal_set signals_;
    // 管理所有http会话
    SessionController session_controller_;
    // 用于处理请求
    RequestHandler request_handler_;
    // 发出接收请求
    void do_accept();
    // 结束时，graceful shutdown
    void do_await_shutdown();
    // 分配请求到新的IO context
    IO_Ctx_Ptr &get_io_context();
};

}// namespace http