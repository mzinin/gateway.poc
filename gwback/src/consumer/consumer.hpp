#pragma once

#include <message/message.hpp>
#include <utils/config.hpp>

#include <string>
#include <vector>


class MessageConsumer final
{
public:
    MessageConsumer(const ConsumerConfig& config);
    ~MessageConsumer();

    bool operator()(Messages& messages);
    void wait() const;

private:
    void flushBuffer();

private:
    const uint32_t chunkSize_;
    const std::string outputFolder_;
    Messages buffer_;
};
