#pragma once

#include <common/utils/config.hpp>

#include <pqxx/pqxx>

#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>


class PostgresWriter final
{
public:
    using ConfigType = common::PostgresConfig;

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
    PostgresWriter(const common::PostgresConfig& config);

    Result write(const std::string& data);

private:
    pqxx::connection& findConnection();

private:
    const std::string connectionString_;

    struct ExclusiveConnection
    {
        std::thread::id threadId;
        std::unique_ptr<pqxx::connection> connection;
    };
    std::vector<ExclusiveConnection> connections_;
    std::shared_mutex mutex_;
};
