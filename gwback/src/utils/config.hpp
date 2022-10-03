#pragma once

#include <common/utils/config.hpp>

#include <cstdint>
#include <optional>
#include <string_view>


struct ConsumerConfig final
{
    uint32_t chunkSize = 1000;
    std::string outputFolder;
};

struct ProducerConfig final
{
    uint32_t chunkSize = 1000;
    uint32_t messageLimit = 1000000;
};

struct Config final
{
    common::LogConfig log;
    std::optional<common::PostgresConfig> postgres;
    std::optional<common::RedisConfig> redis;
    ConsumerConfig consumer;
    ProducerConfig producer;
};

bool parseConfig(std::string_view filePath, Config& config);
