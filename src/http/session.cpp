#include <http/session.hpp>
#include <utils/log.hpp>

#include <chrono>


namespace
{
    using Request = boost::beast::http::request<boost::beast::http::string_body>;
    using Response = boost::beast::http::response<boost::beast::http::string_body>;

    Response handleRequest(Request&& request)
    {
        // TODO handle requests

        Response response{boost::beast::http::status::bad_request, request.version()};

        response.set(boost::beast::http::field::content_type, "text/json");
        response.keep_alive(request.keep_alive());
        response.body() = R"({"error": "I cannot handle requests yet"})";
        response.prepare_payload();

        return response;
    }
}

HttpSession::HttpSession(uint16_t timeout, boost::asio::ip::tcp::socket&& socket)
    : timeout_(timeout)
    , stream_(std::move(socket))
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

    response_ = handleRequest(std::move(request_));
    boost::beast::http::async_write(stream_,
                                    response_,
                                    boost::beast::bind_front_handler(&HttpSession::onWrite, shared_from_this()));
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
