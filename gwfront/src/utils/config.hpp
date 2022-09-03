#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

struct LogConfig final
{
    std::string severity = "error";
};

struct HttpServerConfig final
{
    uint16_t port;
    uint16_t threads;
    uint16_t requestTimeout = 10; // seconds
};

struct PostgresConfig final
{
    std::string host;
    uint16_t port = 5432;
    std::string database;
    std::string user;
    std::string password;
};

struct Config final
{
    LogConfig log;
    HttpServerConfig http;
    std::optional<PostgresConfig> postgres;
};

bool parseConfig(std::string_view filePath, Config& config);
