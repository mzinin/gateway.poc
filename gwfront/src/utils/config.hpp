#pragma once

#include <common/utils/config.hpp>

#include <cstdint>
#include <optional>
#include <string>
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
    HttpServerConfig http;
    std::optional<common::PostgresConfig> postgres;
};

bool parseConfig(std::string_view filePath, Config& config);
