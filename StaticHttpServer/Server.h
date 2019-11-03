#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

class Server
{
public:
    // �������ƹ��죬Ĭ�Ϲ���Ҳ��ɾ����
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // ǿ����ʽ����
    explicit Server(short port);
    // ���õ�
    void Run();

    ~Server() {}

private:
    // ����IO�¼���Ӧ������
    boost::asio::io_context io_context_;
    // �����˿��ϵ�TCP����
    boost::asio::ip::tcp::acceptor acceptor_;
    // ���ڴ�������쳣�˳�����ǿ�Ʊ��رս���
    boost::asio::signal_set signals_;
    // ��������http�Ự
    SessionController session_controller_;
    // ���ڴ�������
    RequestHandler request_handler_;
    // ���������е��߳���
    int nr_threads_;
    // ������������
    void do_accept();
    // ����ʱ��graceful shutdown
    void do_shutdown();
};

}// namespace http