#pragma once

#include <cstdint>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>


class HttpSession final : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(uint16_t timeout, boost::asio::ip::tcp::socket&& socket);
    ~HttpSession();

    void run();

private:
    void read();
    void onRead(boost::beast::error_code ec, size_t bytesRead);
    void onWrite(boost::beast::error_code ec, size_t bytesWritten);
    void close();

private:
    uint16_t timeout_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
};
