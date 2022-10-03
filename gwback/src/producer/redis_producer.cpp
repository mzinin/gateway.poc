#include <common/utils/log.hpp>
#include <common/utils/resolve_host.hpp>
#include <producer/redis_producer.hpp>

#include <cctype>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <vector>


namespace
{
    constexpr std::string_view MESSAGES_STREAM = "messages";
    constexpr std::string_view DATA_FIELD = "data";

    constexpr size_t EXPECTED_ENTRY_SIZE = 2;
    constexpr uint64_t TIMESTAMP_SHIFT = 22; // bits
    constexpr uint64_t SEQUENCE_MASK = (1 << TIMESTAMP_SHIFT) - 1;

    uint64_t messageIdFromReply(const redisReply* reply)
    {
        if (reply->type != REDIS_REPLY_STRING)
        {
            throw std::runtime_error("Unexpected type of id reply: " + std::to_string(reply->type));
        }

        // id string consists of timestamp and sequence number delimited by '-': '1662737151107-0'
        size_t i = 0;
        uint64_t ts = 0;
        for (; i < reply->len && std::isdigit(reply->str[i]); ++i)
        {
            ts = ts * 10 + (reply->str[i] - '0');
        }

        ++i;
        uint64_t seq = 0;
        for (; i < reply->len && std::isdigit(reply->str[i]); ++i)
        {
            seq = seq * 10 + (reply->str[i] - '0');
        }

        return (ts << TIMESTAMP_SHIFT) | seq;
    }

    std::vector<uint8_t> messageDataFromReply(const redisReply* reply)
    {
        if (reply->type != REDIS_REPLY_ARRAY )
        {
            throw std::runtime_error("Unexpected type of data reply: " + std::to_string(reply->type));
        }

        for (size_t i = 0; i < reply->elements; i += 2)
        {
            const auto* fieldName = reply->element[i];
            const auto* fieldValue = reply->element[i + 1];

            if (fieldName->type == REDIS_REPLY_STRING &&
                std::string_view{fieldName->str, fieldName->len} == DATA_FIELD &&
                fieldValue->type == REDIS_REPLY_STRING)
            {
                return {fieldValue->str, fieldValue->str + fieldValue->len};
            }
        }

        return {};
    }

    Messages messagesFromReply(const redisReply* reply)
    {
        if (reply->type != REDIS_REPLY_ARRAY)
        {
            throw std::runtime_error("Unexpected type of Redis reply: " + std::to_string(reply->type));
        }

        Messages result;
        for (size_t i = 0; i < reply->elements; ++i)
        {
            const auto* entry = reply->element[i];
            if (entry->type != REDIS_REPLY_ARRAY)
            {
                throw std::runtime_error("Unexpected type of Redis stream entry: " + std::to_string(entry->type));
            }
            if (entry->elements != EXPECTED_ENTRY_SIZE)
            {
                throw std::runtime_error("Unexpected size of Redis stream entry: " + std::to_string(entry->elements));
            }

            auto id = messageIdFromReply(entry->element[0]);
            auto data = messageDataFromReply(entry->element[1]);
            result.emplace_back(Message{id, std::move(data)});
        }
        return result;
    }

    std::string makeXdelCommand(const Messages& messages)
    {
        std::ostringstream command;
        command << "XDEL " << MESSAGES_STREAM;

        for (const auto& message : messages)
        {
            command << ' ' << (message.id >> TIMESTAMP_SHIFT) << '-' << (message.id & SEQUENCE_MASK);
        }
        return command.str();
    }
}

RedisProducer::RedisProducer(const common::RedisConfig& redisConfig, const ProducerConfig& producerConfig)
    : config_(redisConfig)
    , chunkSize_(producerConfig.chunkSize)
    , messageLimit_(producerConfig.messageLimit)
    , connection_(nullptr, nullptr)
{
    config_.host = common::resolveHost(redisConfig.host);
    if (config_.host.empty())
    {
        throw std::invalid_argument("Failed to resolve '" + redisConfig.host + "'");
    }
}

Messages RedisProducer::getNext()
{
    auto* connection = getConnection();

    // try send a command
    auto reply = runCommand(connection,
                            "XRANGE %s - + COUNT %u",
                            MESSAGES_STREAM.data(),
                            chunkSize_);

    // reconnect and try again if needed
    if (connection->err == REDIS_ERR_EOF || connection->err == REDIS_ERR_IO)
    {
        reconnect();
        reply = runCommand(connection,
                           "XRANGE %s - + COUNT %u",
                           MESSAGES_STREAM.data(),
                           chunkSize_);
    }

    // check connection state
    if (connection->err != REDIS_OK)
    {
        throw std::runtime_error(std::string("Failed to read from Redis: ") + connection->errstr);
    }

    // check reply
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        throw std::runtime_error(std::string("Failed to read from Redis: ") + reply->str);
    }
    if (!reply)
    {
        // this case should be handled by checking connection state
        throw std::runtime_error("Failed to read from Redis: no details known");
    }

    const auto result = messagesFromReply(reply.get());
    messageCount_ += result.size();
    return result;
}

void RedisProducer::markConsumed(const Messages& messages)
{
    auto* connection = getConnection();
    const auto xdelCommand = makeXdelCommand(messages);

    // try send a command
    auto reply = runCommand(connection, xdelCommand);

    // reconnect and try again if needed
    if (connection->err == REDIS_ERR_EOF)
    {
        reconnect();
        reply = runCommand(connection, xdelCommand);
    }

    // check connection state
    if (connection->err != REDIS_OK)
    {
        throw std::runtime_error(std::string("Failed to read from Redis: ") + connection->errstr);
    }

    // check reply
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        throw std::runtime_error(std::string("Failed to read from Redis: ") + reply->str);
    }
}

bool RedisProducer::done() const
{
    return messageCount_ == messageLimit_;
}

redisContext* RedisProducer::getConnection()
{
    if (!connection_)
    {
        auto* connection = redisConnect(config_.host.data(), config_.port);
        if (!connection)
        {
            throw std::runtime_error("Failed to connect to Redis");
        }
        if (connection->err != REDIS_OK)
        {
            auto message = std::string("Failed to connect to Redis: ") + connection->errstr;
            redisFree(connection);
            throw std::runtime_error(std::move(message));
        }
        Log(debug) << "Connection to Redis established";

        connection_ = ConnectionPtr(connection, redisFree);
        authenticate();
    }
    return connection_.get();
}

void RedisProducer::reconnect()
{
    auto* connection = getConnection();
    if (redisReconnect(connection) != REDIS_OK)
    {
        throw std::runtime_error("Failed to reconnect to Redis");
    }
    Log(debug) << "Connection to Redis reestablished";

    authenticate();
}

void RedisProducer::authenticate()
{
    auto* connection = getConnection();
    auto reply = runCommand(connection, "AUTH %s %s", config_.user.data(), config_.password.data());

    // check connection state
    if (connection->err != REDIS_OK)
    {
        throw std::runtime_error(std::string("Failed to authenticate in Redis: ") + connection->errstr);
    }

    // check reply
    if (reply && reply->type == REDIS_REPLY_ERROR)
    {
        throw std::runtime_error(std::string("Failed to authenticate in Redis: ") + reply->str);
    }

    Log(debug) << "Connection to Redis authenticated";
}
