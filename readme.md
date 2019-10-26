# StaticHttpServer

## Introduction

Cpp编写的静态资源HTTP服务器，计划支持：

1. 高并发
2. keep-alive持久连接
3. GET/POST请求静态文件资源，大文件分块传输
4. HTTPS支持

此项目正在开发中......

## Platform

在Visual Studio 2017上开发Windows版本，由于asio的跨平台特性，可移植至Linux平台。

## How to build

将项目克隆到本地，再用VS打开StaticHttpServer.sln即可，注意选择x64开发。

```
git clone https://github.com/ZiJiaW/StaticHttpServer.git
cd StaticHttpServer
```

### Boost

去[下载地址](https://sourceforge.net/projects/boost/files/boost-binaries/1.71.0/)选择boost_1_71_0-msvc-14.1-64.exe版本安装（14.1对应VS2017版本，其余版本需自行查找对应的msvc版本号），安装完成后，进入项目属性，在**配置属性**--->**VC++目录**中：

* 在**包含目录**中添加：“安装目录\boost_1_71_0”
* 在**库目录**中添加以下依赖：“安装目录\boost_1_71_0\lib64-msvc-14.1”

### openSSL

openSSL的windows版本在[这里](https://slproweb.com/products/Win32OpenSSL.html)有预编译版本，选择64位专业版Win64 OpenSSL v1.1.1d安装，完成后：

* 在**包含目录**中添加：“安装目录\OpenSSL-Win64\include”
* 在**库目录**中添加：“安装目录\OpenSSL-Win64\lib”
* 在**链接器-输入-附加依赖项**中添加：libssl.lib, libcrypto.lib

### Test

尝试编译运行无报错即可。