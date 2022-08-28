#pragma once

#include <handler/interface.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <cstdint>
#include <memory>


class HttpSession final : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(boost::asio::ip::tcp::socket&& socket, uint16_t timeout, IHandler& handler_);
    ~HttpSession();

    void run();

private:
    void read();
    void onRead(boost::beast::error_code ec, size_t bytesRead);
    void handleRequest();
    void onWrite(boost::beast::error_code ec, size_t bytesWritten);
    void close();

private:
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;

    uint16_t timeout_;
    IHandler& handler_;

    boost::beast::http::request<boost::beast::http::string_body> request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
};
