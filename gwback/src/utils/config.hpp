#pragma once

#include <common/utils/config.hpp>

#include <optional>
#include <string_view>


struct Config final
{
    common::LogConfig log;
    std::optional<common::PostgresConfig> postgres;
};

bool parseConfig(std::string_view filePath, Config& config);
