#include "pch.h"
#include "Server.h"

namespace http {

Server::Server(short port, int nr_threads):
    nr_threads_(nr_threads),
    io_contexts_(),
    acceptor_(get_io_context()),
    signals_(get_io_context()),
    session_controller_()
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    do_await_shutdown();// 注册结束事件

    namespace ip = boost::asio::ip;
    boost::system::error_code ec;
    auto endpoint = ip::tcp::endpoint(ip::tcp::v4(), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(ip::tcp::endpoint(ip::tcp::v4(), port), ec);
    if (ec) {
        std::cerr << "Failed when opening the port! Please check!" << std::endl;
        exit(1);
    }

    acceptor_.listen();
    do_accept();// 注册接收新的连接事件
}

// 为每个IO Context创建一个线程执行
void Server::Run()
{
    std::vector<std::shared_ptr<std::thread>> thread_pool;
    for (int i = 0; i < nr_threads_; ++i) {
        thread_pool.push_back(std::make_shared<std::thread>(
        [this, i]() {
            io_contexts_[i]->run();
        }));
    }
    for (auto thp : thread_pool)
        thp->join();
}

void Server::do_accept()
{

}

void Server::do_await_shutdown()
{

}

// 轮转调度线程来分配任务
IO_Ctx_Ptr& Server::get_io_context()
{
    if (io_contexts_.empty()) {
        for (int i = 0; i < nr_threads_; ++i) {
            io_contexts_.push_back(std::make_shared<boost::asio::io_context>(1));
        }
    }
    static unsigned int count = 0;
    if (count >= io_contexts_.size()) count = 0;
    return io_contexts_[count];
}

}// namespace http