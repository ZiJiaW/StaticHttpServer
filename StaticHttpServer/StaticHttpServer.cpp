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

    /*std::ifstream file("./index.html", std::ios::in | std::ios::binary);
    char buffer[1024];
    file.read(buffer, 1024);
    std::cout << buffer << std::endl;*/

    try {
        http::Server server(8080);
        server.Run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}