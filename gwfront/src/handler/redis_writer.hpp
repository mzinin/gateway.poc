#pragma once

#include <common/utils/config.hpp>

#include <hiredis/hiredis.h>

#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>


class RedisWriter final
{
public:
    using ConfigType = common::RedisConfig;

    enum class Error
    {
        OK,
        GENERIC,
        STORAGE_UNAVAILABLE,
    };

    struct Result
    {
        Error error;
        std::string message;
    };

public:
    RedisWriter(const ConfigType& config);

    Result write(const std::string& data);

private:
    struct CheckedConnection
    {
        redisContext* connection;
        std::string error;
    };

    using ConnectionPtr = std::unique_ptr<redisContext, void(*)(redisContext*)>;

    struct ExclusiveConnection
    {
        std::thread::id threadId;
        ConnectionPtr connection;
    };

private:
    CheckedConnection findConnection();
    CheckedConnection createConnection();
    CheckedConnection reconnect(redisContext* connection);
    CheckedConnection authenticate(redisContext* connection);

private:
    ConfigType config_;
    std::vector<ExclusiveConnection> connections_;
    std::shared_mutex mutex_;
};