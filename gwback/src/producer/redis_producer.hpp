#pragma once

#include <producer/interface.hpp>
#include <utils/config.hpp>

#include <hiredis/hiredis.h>

#include <memory>
#include <string>


class RedisProducer final : public IProducer
{
public:
    RedisProducer(const common::RedisConfig& redisConfig, const ProducerConfig& producerConfig);

    Messages getNext() override;
    void markConsumed(const Messages& messages) override;
    bool done() const override;

private:
    using ConnectionPtr = std::unique_ptr<redisContext, void(*)(redisContext*)>;
    using ReplyPtr = std::unique_ptr<redisReply, void(*)(void*)>;

    redisContext* getConnection();
    void reconnect();
    void authenticate();

    template <class ... Args>
    ReplyPtr runCommand(redisContext* connection, const std::string& format, Args ... args)
    {
        auto* reply = static_cast<redisReply*>(redisCommand(connection, format.c_str(), args...));
        return ReplyPtr(reply, freeReplyObject);
    }

private:
    common::RedisConfig config_;
    const uint32_t chunkSize_;
    const uint32_t messageLimit_;
    uint32_t messageCount_ = 0;
    ConnectionPtr connection_;
};
