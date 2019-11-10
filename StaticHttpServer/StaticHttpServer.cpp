// StaticHttpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "Server.h"

extern "C" {
#include <openssl/applink.c>
}

int main()
{
    // test

    std::cout << "Start Test!" << std::endl;

    try {
        http::Server server(8080);
        server.Run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}