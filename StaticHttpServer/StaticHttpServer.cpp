// StaticHttpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

extern "C" {
#include <openssl/applink.c>
}

using boost::system::error_code;
using namespace boost::asio;

class session : public std::enable_shared_from_this<session> {
public:
    session(ip::tcp::socket socket): _socket(std::move(socket)) {}
    void Start() {
        DoRead();
    }

private:
    ip::tcp::socket _socket;
    char buf[1024];

    void DoRead() {
        auto self(shared_from_this());
        _socket.async_read_some(buffer(buf), [this, self](error_code ec, std::size_t len) {
            if (!ec) DoWrite(len);
        });
    }

    void DoWrite(std::size_t len) {
        auto self(shared_from_this());
        _socket.async_write_some(buffer(buf, len), [this, self](error_code ec, std::size_t len) {
            if (!ec) DoRead();
        });
    }
};

class Server {
public:
    Server(io_context& ioc, short port): _acceptor(ioc, ip::tcp::endpoint(ip::tcp::v4(), port)) {
        DoAccept();
    }

private:
    ip::tcp::acceptor _acceptor;

    void DoAccept() {
        _acceptor.async_accept([this](error_code ec, ip::tcp::socket socket) {
            if (!ec) std::make_shared<session>(std::move(socket))->Start();
            DoAccept();
        });
    }
};

int main()
{
    // openssl rsa test
    auto r = RSA_new();
    auto e = BN_new();
    BN_set_word(e, RSA_F4);
    if (RSA_generate_key_ex(r, 1024, e, NULL) != 1) {
        std::cout << "error happened when generating rsa!" << std::endl;
    }
    else {
        RSA_print_fp(stdout, r, 0);
    }
    RSA_free(r);

    // simple tcp echo server

    try {
        io_context ioc;
        Server server(ioc, 8080);
        ioc.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}