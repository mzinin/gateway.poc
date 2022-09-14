#pragma once

#include <common/utils/config.hpp>

#include <cstdint>
#include <optional>
#include <string_view>


struct HttpServerConfig final
{
    uint16_t port;
    uint16_t threads;
    uint16_t requestTimeout = 10; // seconds
};

struct Config final
{
    common::LogConfig log;
    common::MessagesConfig messages;
    HttpServerConfig http;
    std::optional<common::PostgresConfig> postgres;
    std::optional<common::RedisConfig> redis;
};

bool parseConfig(std::string_view filePath, Config& config);
