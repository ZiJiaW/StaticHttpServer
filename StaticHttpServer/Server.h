#pragma once
#include "SessionController.h"
#include "RequestHandler.h"

namespace http {

using IO_Ctx_Ptr = std::shared_ptr<boost::asio::io_context>;
using Rh_Ptr = std::shared_ptr<RequestHandler>;

class Server
{
public:
    // �������ƹ��죬Ĭ�Ϲ���Ҳ��ɾ����
    Server(const Server &server) = delete;
    Server &operator=(const Server &server) = delete;

    // Ĭ�ϰ��մ������߳����������̳߳�
    explicit Server(short port, int nr_threads = std::thread::hardware_concurrency());
    // ���õ�
    void Run();

    ~Server() = default;

private:
    // �߳���
    int nr_threads_;
    // �������IO�¼���Ӧ������
    std::vector<IO_Ctx_Ptr> io_contexts_;
    using IO_Ctx_Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    std::vector<IO_Ctx_Work> works_;
    // ���ڴ�������
    std::vector<Rh_Ptr> handlers_;
    // �����˿��ϵ�TCP����
    boost::asio::ip::tcp::acceptor acceptor_;
    // ���ڴ�������쳣�˳�����ǿ�Ʊ��رս���
    boost::asio::signal_set signals_;
    // ��������http�Ự
    SessionController session_controller_;
    // ������������
    void do_accept();
    // ����ʱ��graceful shutdown
    void do_await_shutdown();
    // ���������µ�IO context
    std::tuple<IO_Ctx_Ptr, Rh_Ptr> get_io_context();
};

}// namespace http