#include <http/session.hpp>
#include <utils/log.hpp>

#include <chrono>
#include <string_view>


namespace
{
    constexpr std::string_view INSTRUMENT_PATH = "/instrument";
    constexpr std::string_view JSON_CONTENT_TYPE = "text/json";
}

HttpSession::HttpSession(boost::asio::ip::tcp::socket&& socket, uint16_t timeout, IHandler& handler)
    : stream_(std::move(socket))
    , timeout_(timeout)
    , handler_(handler)
{
    const auto endpoint = stream_.socket().remote_endpoint();
    Log(info) << "Incoming connection from " << endpoint.address().to_string() << ":" << endpoint.port();
}

HttpSession::~HttpSession()
{
    boost::beast::error_code ec;
    stream_.socket().close(ec);

    if (ec)
    {
        Log(warning) << "Failed to close tcp socket: " << ec.message();
    }
}

void HttpSession::run()
{
    boost::asio::dispatch(stream_.get_executor(),
                          boost::beast::bind_front_handler(&HttpSession::read, shared_from_this()));
}

void HttpSession::read()
{
    request_ = {};
    stream_.expires_after(std::chrono::seconds(30));

    boost::beast::http::async_read(stream_,
                                   buffer_,
                                   request_,
                                   boost::beast::bind_front_handler(&HttpSession::onRead, shared_from_this()));
}

void HttpSession::onRead(boost::beast::error_code ec, size_t /*bytesRead*/)
{
    if (ec == boost::beast::http::error::end_of_stream)
    {
        close();
        return;
    }

    if (ec)
    {
        Log(error) << "Failed to read request: " << ec.message();
        return;
    }

    handleRequest();
    boost::beast::http::async_write(stream_,
                                    response_,
                                    boost::beast::bind_front_handler(&HttpSession::onWrite, shared_from_this()));
}

void HttpSession::handleRequest()
{
    response_ = {};
    response_.version(request_.version());
    response_.keep_alive(request_.keep_alive());
    response_.set(boost::beast::http::field::content_type, JSON_CONTENT_TYPE);

    if (request_.method() != boost::beast::http::verb::post)
    {
        response_.result(boost::beast::http::status::method_not_allowed);
        response_.body() = R"({"error": "wrong HTTP method"})";
    }
    else if (request_.target() != INSTRUMENT_PATH)
    {
        response_.result(boost::beast::http::status::bad_request);
        response_.body() = R"({"error": "wrong path"})";
    }
    else
    {
        auto result = handler_(request_.body());
        switch (result.error)
        {
            case IHandler::Error::OK:
                response_.result(boost::beast::http::status::accepted);
                break;

            case IHandler::Error::BAD_DATA:
                response_.result(boost::beast::http::status::bad_request);
                response_.body() = R"({"error": ")" + result.message + R"("})";
                break;

            case IHandler::Error::STORAGE_UNAVAILABLE:
                response_.result(boost::beast::http::status::service_unavailable);
                response_.body() = R"({"error": ")" + result.message + R"("})";
                break;

            case IHandler::Error::GENERIC:
                response_.result(boost::beast::http::status::internal_server_error);
                response_.body() = R"({"error": ")" + result.message + R"("})";
                break;
        }
    }

    response_.prepare_payload();
}

void HttpSession::onWrite(boost::beast::error_code ec, size_t /*bytesWritten*/)
{
    if (ec)
    {
        Log(error) << "Failed to write response: " << ec.message();
        return;
    }

    if (response_.keep_alive())
    {
        read();
    }

    close();
}

void HttpSession::close()
{
    boost::beast::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

    if (ec)
    {
        Log(trace) << "Failed to close http session: " << ec.message();
    }
}
