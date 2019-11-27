#include "pch.h"
#include "RequestHandler.h"


namespace http {

const std::unordered_map<StatusCode, std::string> response_str_map = {
    {StatusCode::OK, "HTTP/1.1 200 OK\r\n"},
    {StatusCode::CREATED, "HTTP/1.1 201 Created\r\n"},
    {StatusCode::ACCEPTED, "HTTP/1.1 202 Accepted\r\n"},
    {StatusCode::BAD_GATEWAY, "HTTP/1.1 502 Bad Gateway\r\n"},
    {StatusCode::BAD_REQUEST, "HTTP/1.1 400 Bad Request\r\n"},
    {StatusCode::FORBIDDEN, "HTTP/1.1 403 Forbidden\r\n"},
    {StatusCode::INTERNAL_ERROR, "HTTP/1.1 500 Internal Server Error\r\n"},
    {StatusCode::LENGTH_REQUIRED, "HTTP/1.1 411 Length Required\r\n"},
    {StatusCode::METHOD_NOT_ALLOWED, "HTTP/1.1 405 Method Not Allowed\r\n"},
    {StatusCode::NOT_ACCEPTABLE, "HTTP/1.1 406 Not Acceptable\r\n"},
    {StatusCode::NOT_FOUND, "HTTP/1.1 404 Not Found\r\n"},
    {StatusCode::NOT_IMPLEMENTED, "HTTP/1.1 501 Not Implemented\r\n"},
    {StatusCode::NO_CONTENT, "HTTP/1.1 204 No Content\r\n"},
    {StatusCode::REQUEST_TIMEOUT, "HTTP/1.1 408 Request Timeout\r\n"},
    {StatusCode::SERVICE_UNAVAILABLE, "HTTP/1.1 503 Service Unavailable\r\n"},
    {StatusCode::UNAUTHORIZED, "HTTP/1.1 401 Unauthorized\r\n"},
    {StatusCode::VERSION_NOT_SUPPORTED, "HTTP/1.1 505 Version Not Supported\r\n"},
};

const std::unordered_map<std::string, std::string> mime_map = {
    {"gif", "image/gif"},
    {"jpg", "image/jpeg"},
    {"htm", "text/htm"},
    {"html", "text/html"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"zip", "application/zip"},
    {"js", "text/javascript"},
    {"css", "text/css"},
    // 后续可考虑添加视频格式
};

std::string RequestHandler::HandleBadRequest(StatusCode code)
{
    Response rs;
    rs.set_headline(response_str_map.at(code));
    std::string body = "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>" + std::to_string(static_cast<int>(code)) +
        " Error from SHS</h1><h2>" + response_str_map.at(code) + "<h2></body>"
        "</html>";
    rs.set_header("Content-Length", std::to_string(body.size()));
    rs.set_header("Content-Type", "text/html");
    rs.set_header("Server", "SHS");
    rs.set_body(std::move(body));
    return rs.ToString();
}

void RequestHandler::HandleGetRequest(Request &req, std::function<void(const std::string &)> call_back)
{
    if (!req.ok()) {
        call_back(HandleBadRequest(req.status()));
        return;
    }
    Response rs;
    auto file_path = root_path + decode(req.uri());
    if (*file_path.rbegin() == '/') {
        file_path += "index.html";
    }
    auto last_dot = file_path.find_last_of('.');
    auto last_slash = file_path.find_last_of('/');
    if (last_dot != std::string::npos && last_dot > last_slash) {
        auto file_extension = file_path.substr(last_dot + 1);
        if (mime_map.find(file_extension) == mime_map.end()) {
            rs.set_header("Content-Type", "text/plain");
        }
        else {
            auto tmp = mime_map.at(file_extension);
            rs.set_header("Content-Type", std::move(tmp));
        }
    }
    else {
        call_back(HandleBadRequest(StatusCode::NOT_FOUND));
        return;
    }
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    std::cout << "Try opening " << file_path << std::endl;
    if (!file.is_open()) {
        call_back(HandleBadRequest(StatusCode::NOT_FOUND));
        return;
    }
    char buf[2048];
    do {
        file.read(buf, sizeof(buf));
        rs.push_body(buf, file.gcount());
        //std::cout << rs.body_size() << std::endl;
    } while (!file.eof());
    std::cout << rs.body_size() << std::endl;
    rs.set_header("Content-Length", std::to_string(rs.body_size()));
    rs.set_header("Server", "SHS");
    rs.set_headline(response_str_map.at(StatusCode::OK));
    file.close();
    call_back(rs.ToString());
}

void RequestHandler::PostResponseOK(Response &rs)
{
	rs.set_headline(response_str_map.at(StatusCode::OK));
	std::string body = "<html>"
		"<head><title>POST Request</title></head>"
		"<body><h1>" + std::to_string(static_cast<int>(OK)) +
		" success</h1><h2>" + response_str_map.at(OK) + "<h2></body>"
		"</html>";
	rs.set_header("Content-Length", std::to_string(body.size()));
	rs.set_header("Content-Type", "text/html");
	rs.set_header("Server", "SHS");
	rs.set_body(std::move(body));
}

std::size_t RequestHandler::find_next(const std::string &str, const std::string &aim, std::size_t pos)
{
    auto next = str.find(aim, pos);
    if (next != std::string::npos)
        return next + aim.size();
    else return std::string::npos;
}

std::string RequestHandler::HandlePostRequest(Request &req)
{
	Response rs;
	std::string boundary;
	std::string lastboundary;
	//当没有选择文件时Content-Type属性不存在直接返回200ok
	try
	{
		req.get("Content-Type");
	}
	catch (std::out_of_range &exc)
	{
		PostResponseOK(rs);
		return rs.ToString();
	}

	auto Type = req.get("Content-Type");
	auto pos = Type.find("boundary=");
	if (pos != std::string::npos)     //取得分界符和结束符
	{
		std::string b = "boundary=";
		pos += b.size();
		boundary = "--" + Type.substr(pos);
		lastboundary = boundary + "--";
	}
	
	std::string reqcontent = req.get_body();
	//当选中文件又取消则有Content-Type属性但body内为空直接返回200ok
	if (reqcontent.size() < lastboundary.size())
	{
		PostResponseOK(rs);
		return rs.ToString();
	}

    std::string::size_type loc = 0;
	while ((loc = find_next(reqcontent, "filename=", loc)) != std::string::npos)
	{
        // get file name
        auto name_start = find_next(reqcontent, "\"", loc);
        loc = find_next(reqcontent, "\"", name_start);
        auto filename = reqcontent.substr(name_start, loc - name_start - 1);
        
        //std::cout << filename << std::endl;
        // 两个连续的\r\n构成文件流的起点
        auto content_start = find_next(reqcontent, "\r\n\r\n", loc);
        // 而后直接寻找下一个boundary
        loc = find_next(reqcontent, boundary, content_start);
        std::size_t content_length = 0;
        // 如果没找到说明到底了，找lastboundary
        if (loc != std::string::npos) {
            content_length = loc - content_start - boundary.size() - 2;
        }
        else {
            content_length = reqcontent.find(lastboundary, content_start) - content_start - 2;
        }

		auto file_path = root_path + "/" + filename;

		std::ofstream file(file_path, std::ios::out | std::ios::binary);

        // 这里用指针的形式写文件，也防止了一次复制
        file.write(reqcontent.c_str() + content_start, content_length);
        // 注意文件流的关闭
        file.close();
	}

	PostResponseOK(rs);
    return rs.ToString();
}

std::string RequestHandler::decode(const std::string &uri)
{// uri example: /index.html?key1=1225123   /hello   /
// it's only a static server, so we don't handle parameters
    auto pos = uri.find('?');
    return std::string(uri, 0, pos);
}
}// namespace http
