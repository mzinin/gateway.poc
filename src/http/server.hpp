#pragma once

#include <handler/interface.hpp>
#include <utils/config.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <thread>
#include <vector>


class HttpServer final
{
public:
    HttpServer(const HttpServerConfig& config, IHandler& handler);

    void start();
    void stop();

private:
    void listen();
    void accept();

    void runWorkers();
    void joinWorkers();

    void onAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

private:
    HttpServerConfig config_;
    boost::asio::io_context ioContext_;
    boost::asio::ip::tcp::acceptor acceptor_;
    IHandler& handler_;

    std::atomic_bool stopping_;

    std::vector<std::thread> workers_;
};
