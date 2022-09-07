#include <common/utils/log.hpp>
#include <common/utils/resolve_host.hpp>
#include <handler/redis_writer.hpp>

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <string_view>


namespace
{
    constexpr std::string_view MESSAGES_STREAM = "messages";
    constexpr std::string_view DATA_FIELD = "data";
}

RedisWriter::RedisWriter(const ConfigType& config)
    : config_(config)
{
    config_.host = common::resolveHost(config.host);
    if (config_.host.empty())
    {
        throw std::invalid_argument("Failed to resolve '" + config.host + "'");
    }
}

RedisWriter::Result RedisWriter::write(const std::string& data)
{
    RedisWriter::Result result{Error::OK, ""};

    // find or create a connection
    auto checkedConnection = findConnection();
    if (!checkedConnection.connection)
    {
        result.error = Error::STORAGE_UNAVAILABLE;
        result.message = std::move(checkedConnection.error);
        return result;
    }

    // try send a command
    auto* reply = static_cast<redisReply*>(redisCommand(checkedConnection.connection,
                                                        "XADD %s * %s %b",
                                                        MESSAGES_STREAM.data(),
                                                        DATA_FIELD.data(),
                                                        data.data(), data.size()));

    // reconnect and try again if needed
    if (checkedConnection.connection->err == REDIS_ERR_EOF)
    {
        checkedConnection = reconnect(checkedConnection.connection);
        if (!checkedConnection.connection)
        {
            result.error = Error::STORAGE_UNAVAILABLE;
            result.message = std::move(checkedConnection.error);
            return result;
        }
        reply = static_cast<redisReply*>(redisCommand(checkedConnection.connection,
                                                      "XADD %s * %s %b",
                                                      MESSAGES_STREAM.data(),
                                                      DATA_FIELD.data(),
                                                      data.data(), data.size()));
    }

    // check connection state
    switch (checkedConnection.connection->err)
    {
        case REDIS_ERR_EOF:
            result.error = Error::STORAGE_UNAVAILABLE;
            result.message = checkedConnection.connection->errstr;
            break;

        case REDIS_OK:
            break;

        default:
            result.error = Error::GENERIC;
            result.message = checkedConnection.connection->errstr;
            break;
    }

    // check reply
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        result.error = Error::GENERIC;
        result.message = reply->str;
    }

    freeReplyObject(reply);
    return result;
}

RedisWriter::CheckedConnection RedisWriter::findConnection()
{
    // try to find existing connection
    {
        std::shared_lock lock(mutex_);
        auto it = std::find_if(connections_.begin(),
                               connections_.end(),
                               [threadId = std::this_thread::get_id()](const auto& ec){ return ec.threadId == threadId; });
        if (it != connections_.end())
        {
            return {it->connection.get(), std::string{}};
        }
    }

    // create a new connection
    {
        auto createdConnection = createConnection();
        if (!createdConnection.connection)
        {
            return {nullptr, std::move(createdConnection.error)};
        }
        else
        {
            std::unique_lock lock(mutex_);
            connections_.push_back({
                .threadId = std::this_thread::get_id(),
                .connection = ConnectionPtr(createdConnection.connection, redisFree)
            });
            return {createdConnection.connection, std::string{}};
        }
    }
}

RedisWriter::CheckedConnection RedisWriter::createConnection()
{
    auto* connection = redisConnect(config_.host.data(), config_.port);

    if (!connection)
    {
        return {nullptr, "Failed to connect to redis"};
    }

    if (connection->err != REDIS_OK)
    {
        CheckedConnection result{nullptr, connection->errstr};
        redisFree(connection);
        return result;
    }
    Log(debug) << "Connection to Redis established";

    auto result = authenticate(connection);
    if (!result.connection)
    {
        redisFree(connection);
        Log(error) << "Failed to authenticate Redis connection";
    }
    return result;
}

RedisWriter::CheckedConnection RedisWriter::reconnect(redisContext* connection)
{
    if (redisReconnect(connection) != REDIS_OK )
    {
        return {nullptr, "Failed to reconnect to redis"};
    }
    return authenticate(connection);
}

RedisWriter::CheckedConnection RedisWriter::authenticate(redisContext* connection)
{
    auto* reply = static_cast<redisReply*>(redisCommand(connection,
                                                        "AUTH %s %s",
                                                        config_.user.data(),
                                                        config_.password.data()));

    // check connection state
    if (connection->err != REDIS_OK)
    {
        return {nullptr, connection->errstr};
    }

    // check reply
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        CheckedConnection result{nullptr, reply->str};
        freeReplyObject(reply);
        return result;
    }

    freeReplyObject(reply);
    return {connection, std::string{}};
}
