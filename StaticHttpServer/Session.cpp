#include "pch.h"
#include "Session.h"
#include "SessionController.h"


namespace http {

Session::Session(boost::asio::ip::tcp::socket socket,
    boost::asio::io_context &io_context,
    SessionController &session_controller,
    RequestHandler &request_handler) :
    socket_(std::move(socket)),
    io_context_(io_context),
    session_controller_(session_controller),
    request_handler_(request_handler)
{}

void Session::Open()
{
    do_read();
}

void Session::Close()
{
    std::cout << "Close connection: " 
        << socket_.remote_endpoint().address()
        << ":"<< socket_.remote_endpoint().port() 
        << std::endl;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    socket_.close();
}

void Session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_), 
    [this, self](boost::system::error_code ec, std::size_t nr_bytes)
    {
        std::cout << "start read!" << std::endl;
        if (ec) {// 最简单的错误处理
            std::cout << "Bad reading attempt: " << ec.message() << std::endl;
            session_controller_.Stop(shared_from_this());
            return;
        }
        std::cout << std::string(buffer_.data(), nr_bytes) << std::endl;
        auto parse_result = parser_.Parse(buffer_.data(), buffer_.data() + nr_bytes, request_);
        switch (parse_result) {
        case RequestParser::ParseResult::BAD:
            // 这里的bad是仅表示请求格式错误
            response_ = request_handler_.HandleBadRequest(StatusCode::BAD_REQUEST);
            parser_.Reset();
            do_write();
            break;
        case RequestParser::ParseResult::GOOD:
            // 进一步解析请求构造应答
            to_close_ = !request_.keep_alive();
            response_ = request_handler_.HandleGoodRequest(request_);
            parser_.Reset();
            do_write();
            break;
        default:// UNFINISHED
            do_read();
        }
    });
}

void Session::do_write()
{
    auto self(shared_from_this());
    std::cout << "start write!" << std::endl;
    // TODO: 这里暂未考虑chunk发送的接口处理
    socket_.async_write_some(boost::asio::buffer(response_),
    [this, self](boost::system::error_code ec, std::size_t nr_bytes)
    {
        if (ec) {
            std::cout << "Bad writing attempt: " << ec.message() << std::endl;
            session_controller_.Stop(shared_from_this());
            return;
        }
        // 发送成功后，如果被要求close connection则关闭
        // 否则默认保持keep alive的长连接
        if (to_close_) {
            session_controller_.Stop(shared_from_this());
            return;
        }
        // 继续读新请求
        // TODO: 要考虑超时处理
        do_read();
    });
}

}