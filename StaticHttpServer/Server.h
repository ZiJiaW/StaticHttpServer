#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

using IO_Ctx_Ptr = std::shared_ptr<boost::asio::io_context>;

class Server
{
public:
    // �������ƹ��죬Ĭ�Ϲ���Ҳ��ɾ����
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // ǿ����ʽ����
    // Ĭ�ϰ��մ������߳����������̳߳�
    explicit Server(short port, int nr_threads = std::thread::hardware_concurrency());
    // ���õ�
    void Run();

    ~Server() {}

private:
    // �߳���
    int nr_threads_;
    // �������IO�¼���Ӧ������
    std::vector<IO_Ctx_Ptr> io_contexts_;
    // �����˿��ϵ�TCP����
    boost::asio::ip::tcp::acceptor acceptor_;
    // ���ڴ�������쳣�˳�����ǿ�Ʊ��رս���
    boost::asio::signal_set signals_;
    // ��������http�Ự
    SessionController session_controller_;
    // ���ڴ�������
    RequestHandler request_handler_;
    // ������������
    void do_accept();
    // ����ʱ��graceful shutdown
    void do_await_shutdown();
    // ���������µ�IO context
    IO_Ctx_Ptr &get_io_context();
};

}// namespace http