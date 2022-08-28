#pragma once

#include <cstdint>
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

struct Config final
{
    LogConfig log;
    HttpServerConfig http;
};

bool parseConfig(std::string_view filePath, Config& config);
