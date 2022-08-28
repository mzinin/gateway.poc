#include <http/server.hpp>
#include <http/session.hpp>
#include <utils/log.hpp>

#include <memory>


HttpServer::HttpServer(const HttpServerConfig& config, IHandler& handler)
    : config_{config}
    , ioContext_{config_.threads}
    , acceptor_{boost::asio::make_strand(ioContext_)}
    , handler_(handler)
{
}

void HttpServer::start()
{
    Log(info) << "Starting http server on port " << config_.port << " with " << config_.threads << " workers";

    stopping_ = false;

    listen();
    accept();
    runWorkers();

    Log(info) << "Http server started";
}

void HttpServer::stop()
{
    Log(info) << "Stopping http server";

    stopping_ = true;

    acceptor_.close();
    ioContext_.stop();

    joinWorkers();

    Log(info) << "Http server stopped";
}

void HttpServer::listen()
{
    const auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), config_.port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(boost::asio::socket_base::max_listen_connections);
}

void HttpServer::accept()
{
    acceptor_.async_accept(
        boost::asio::make_strand(ioContext_),
        [this](auto ec, auto socket){ onAccept(ec, std::move(socket)); }
    );
}

void HttpServer::runWorkers()
{
    workers_.clear();
    for (uint16_t i = 0; i < config_.threads; ++i)
    {
        workers_.emplace_back([this]{ ioContext_.run(); });
    }
}

void HttpServer::joinWorkers()
{
    for (auto& worker : workers_)
    {
        worker.join();
    }
    workers_.clear();
}

void HttpServer::onAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if(ec)
    {
        Log(error) << "Failed to accept connection: " << ec.message();
        return;
    }

    std::make_shared<HttpSession>(std::move(socket), config_.requestTimeout, handler_)->run();

    if (!stopping_)
    {
        accept();
    }
}

