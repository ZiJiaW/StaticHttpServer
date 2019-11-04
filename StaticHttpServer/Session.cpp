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

void Session::Run()
{

}

}