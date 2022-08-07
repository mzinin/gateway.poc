#pragma once

#include <cstdint>
#include <string_view>


struct HttpServerConfig final
{
    uint16_t port;
    uint16_t threads;
};

class Config final
{
public:
    Config(std::string_view filePath);

    HttpServerConfig http;
};
