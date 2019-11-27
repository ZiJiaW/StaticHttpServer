#include "pch.h"
#include "Session.h"
#include "SessionController.h"


namespace http {

Session::Session(boost::asio::ip::tcp::socket socket,
    boost::asio::io_context &io_context,
    SessionController &session_controller,
    RequestHandler &request_handler,
    boost::asio::ssl::context &ssl_context) :
    socket_(std::move(socket), ssl_context),
    io_context_(io_context),
    session_controller_(session_controller),
    request_handler_(request_handler)
{}

void Session::Open()
{
    do_handshake();
}

void Session::do_handshake()
{
    auto self(shared_from_this());
    socket_.async_handshake(boost::asio::ssl::stream_base::server,
    [this, self](const boost::system::error_code &ec) {
        if (!ec) {
            std::cout << "Open ssl connection @" << socket_.lowest_layer().remote_endpoint().address()
                << ":" << socket_.lowest_layer().remote_endpoint().port() << std::endl;
            do_read();
        }
        else {
           //std::cout << "Handshake failure, abort!" << std::endl;
        }
    });
}

void Session::Close()
{
    std::cout << "Close connection: " 
        << socket_.lowest_layer().remote_endpoint().address()
        << ":"<< socket_.lowest_layer().remote_endpoint().port()
        << std::endl;
    boost::system::error_code ec;
    socket_.lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, ec);
    if (!ec) {
        socket_.lowest_layer().close(ec);
        if (ec) std::cout << "Close err: " << ec.message() << std::endl;
    }
    else std::cout << "Shutdown err: " << ec.message() << std::endl;
}

void Session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_), 
    [this, self](boost::system::error_code ec, std::size_t nr_bytes)
    {
        if (ec) {// 最简单的错误处理
            std::cout << "Bad reading attempt: " << ec.message() << std::endl;
            session_controller_.Stop(shared_from_this());
            return;
        }
        //std::cout << std::string(buffer_.data(), nr_bytes) << std::endl;
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
            if (request_.method() == Method::GET) {
                request_handler_.HandleGetRequest(request_, [this](const std::string & resp) {
                    response_ = resp;
                    parser_.Reset();
                    do_write();
                });
            }
            else {// POST here
                response_ = request_handler_.HandlePostRequest(request_);
                parser_.Reset();
                do_write();
            }
            break;
        default:// UNFINISHED
            do_read();
        }
    });
}

void Session::do_write()
{
    auto self(shared_from_this());
    //std::cout << response_ << std::endl;
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
        request_ = Request();
        do_read();
    });
}

}