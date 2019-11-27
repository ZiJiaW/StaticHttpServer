#include "pch.h"
#include "Server.h"
#include "Session.h"

namespace http {

namespace ip = boost::asio::ip;

Server::Server(short port, int nr_threads):
    nr_threads_(nr_threads),
    io_contexts_(),
    handlers_(),
    acceptor_(*std::get<0>(get_io_context())),
    signals_(*std::get<0>(get_io_context())),
    session_controller_(),
    works_(),
    ssl_context_(boost::asio::ssl::context::sslv23)
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    do_await_shutdown();// 注册结束事件

    // 绑定端口
    boost::system::error_code ec;
    auto endpoint = ip::tcp::endpoint(ip::tcp::v4(), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(ip::tcp::endpoint(ip::tcp::v4(), port), ec);
    if (ec) {
        std::cerr << "Failed when opening the port! Please check!" << std::endl;
        exit(1);
    }
    try {
        acceptor_.listen();
    }
    catch (const std::exception &ex) {
        std::cerr << "Can't listen: " << ex.what() << std::endl;
        exit(1);
    }
    // 设置ssl
    ssl_context_.set_options(boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    ssl_context_.use_private_key_file("private.key", boost::asio::ssl::context::file_format::pem, ec);
    if (ec) {
        std::cerr << "Fail to load private key!" << std::endl;
        exit(1);
    }
    ssl_context_.use_certificate_chain_file("server.crt", ec);
    if (ec) {
        std::cerr << "Fail to load certificate file!" << std::endl;
        exit(1);
    }
    ssl_context_.use_tmp_dh_file("dh2048.pem", ec);
    if (ec) {
        std::cerr << "Fail load dh param file: " + ec.message() << std::endl;
        exit(1);
    }

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
        std::cout << "worker thread " << i << " deployed..." << std::endl;
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
            std::cout << "New connection from " << socket.remote_endpoint().address() << ":" << socket.remote_endpoint().port() << std::endl;
            session_controller_.Start(std::make_shared<Session>(
                std::move(socket), *std::get<0>(tmp), session_controller_, *std::get<1>(tmp), ssl_context_));
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
        for (auto ioc : io_contexts_) {
            ioc->stop();// 这里要先关闭io_context，否则会导致session的重复关闭
        }
        acceptor_.close();
        session_controller_.StopAll();
    });
}

// 轮转调度线程来分配任务
// 这里还为每个线程绑定了一个RequestHandler来消除锁
std::tuple<IO_Ctx_Ptr, Rh_Ptr> Server::get_io_context()
{
    if (io_contexts_.empty()) {
        for (int i = 0; i < nr_threads_; ++i) {
            io_contexts_.push_back(std::make_shared<boost::asio::io_context>(1));
            handlers_.push_back(std::make_shared<RequestHandler>());
            works_.push_back(boost::asio::make_work_guard(*io_contexts_[i]));
        }
    }
    static int count = -1;
    if (++count >= io_contexts_.size()) count = 0;
    return std::make_tuple(io_contexts_[count], handlers_[count]);
}

}// namespace http