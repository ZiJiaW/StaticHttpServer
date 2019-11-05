#include "pch.h"
#include "Session.h"


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
        << ": "<< socket_.remote_endpoint().port() 
        << std::endl;
    socket_.close();
}

void Session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_), 
    [this, self](boost::system::error_code ec, std::size_t nr_bytes)
    {
        if (ec) {
            std::cout << "Bad read!" << std::endl;
            Close();
            return;
        }
        Request req;
        auto parse_result = request_handler_.parser().Parse(buffer_.data(), buffer_.data() + nr_bytes, req);
        if (parse_result = RequestParser::ParseResult::BAD) {
            // 这里的bad是仅表示请求格式错误

        }
    });
}

void Session::do_write()
{

}

}