#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

using IO_Ctx_Ptr = std::shared_ptr<boost::asio::io_context>;
using Rh_Ptr = std::shared_ptr<RequestHandler>;

class Server
{
public:
    // 不允许复制构造，默认构造也是删除的
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // 默认按照处理器线程数来创建线程池
    explicit Server(short port, int nr_threads = std::thread::hardware_concurrency());
    // 调用点
    void Run();

    ~Server() = default;

private:
    // 线程数
    int nr_threads_;
    // 多个运行IO事件响应的载体
    std::vector<IO_Ctx_Ptr> io_contexts_;
    using IO_Ctx_Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    std::vector<IO_Ctx_Work> works_;
    // 用于处理请求
    std::vector<Rh_Ptr> handlers_;
    // 监听端口上的TCP连接
    boost::asio::ip::tcp::acceptor acceptor_;
    // 用于处理程序异常退出，如强制被关闭进程
    boost::asio::signal_set signals_;
    // 管理所有http会话
    SessionController session_controller_;
    // 发出接收请求
    void do_accept();
    // 结束时，graceful shutdown
    void do_await_shutdown();
    // 分配请求到新的IO context
    std::tuple<IO_Ctx_Ptr, Rh_Ptr> get_io_context();
};

}// namespace http