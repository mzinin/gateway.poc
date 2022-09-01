#pragma once

#include <utils/config.hpp>

#include <string>


class PostgresWriter final
{
public:
    using ConfigType = PostgresConfig;

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
    PostgresWriter(const PostgresConfig& config);

    Result write(const std::string& data);

private:
    const std::string connectionString_;
};
