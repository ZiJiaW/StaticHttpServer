#include "pch.h"
#include "Server.h"
#include "Session.h"

namespace http {

namespace ip = boost::asio::ip;

Server::Server(short port, int nr_threads):
    nr_threads_(nr_threads),
    io_contexts_(),
    acceptor_(*std::get<0>(get_io_context())),
    signals_(*std::get<0>(get_io_context())),
    session_controller_()
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    do_await_shutdown();// 注册结束事件

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
    acceptor_.async_accept(*std::get<0>(get_io_context()),
    [this](boost::system::error_code ec, ip::tcp::socket socket) {
        if (!acceptor_.is_open()) {
            return;// 被异常关闭后，不再继续接收
        }
        if (!ec) {
            auto tmp = get_io_context();
            std::make_shared<Session>(std::move(socket), *std::get<0>(tmp),
                session_controller_, *std::get<1>(tmp))->Run();
        }
        else {
            std::cout << "Error happened when connecting! Abort!" << std::endl;
        }
        do_accept();
    });
}

void Server::do_await_shutdown()
{
    signals_.async_wait([this](boost::system::error_code ec, int signo) {
        acceptor_.close();
        session_controller_.StopAll();
    });
}

// 轮转调度线程来分配任务
std::tuple<IO_Ctx_Ptr, Rh_Ptr> Server::get_io_context()
{
    if (io_contexts_.empty()) {
        for (int i = 0; i < nr_threads_; ++i) {
            io_contexts_.push_back(std::make_shared<boost::asio::io_context>(1));
            handlers_.push_back(std::make_shared<RequestHandler>());
        }
    }
    static unsigned int count = 0;
    if (count >= io_contexts_.size()) count = 0;
    return std::make_tuple(io_contexts_[count], handlers_[count]);
}

}// namespace http