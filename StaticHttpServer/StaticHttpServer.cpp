// StaticHttpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

using socket_ptr = boost::shared_ptr<boost::asio::ip::tcp::socket>;

extern "C" {
#include <openssl/applink.c>
}

void connection_handler(socket_ptr sock, const boost::system::error_code &e) {
    if (e) return;
    std::cout << "connection established!" << std::endl;
    while (true) {
        char buf[512];
        boost::system::error_code e;
        size_t len = sock->read_some(boost::asio::buffer(buf), e);
        if (e) {
            std::cout << "connection closed!" << std::endl;
            return;
        }
        if (len > 0) {
            sock->write_some(boost::asio::buffer(buf, len), e);
            if (e) {
                std::cout << "error happened when echoing!" << std::endl;
                sock->close();
                return;
            }
        }
    }
}


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

    // simple tcp echo server test
    boost::asio::io_service service;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 2000);
    boost::asio::ip::tcp::acceptor acc(service, ep);
    socket_ptr sock(new boost::asio::ip::tcp::socket(service));

    acc.async_accept(*sock, boost::bind(connection_handler, sock, boost::placeholders::_1));
    std::cout << "Server listening on " << ep.address() <<":" << ep.port() << std::endl;
    service.run();
}