#include <utils/config.hpp>

#include <tomlplusplus/toml.hpp>

#include <iostream>
#include <stdexcept>

namespace
{
    constexpr std::string_view HTTP_CONFIG_TABLE = "http";
    constexpr std::string_view LOG_CONFIG_TABLE = "log";
    constexpr std::string_view POSTGRES_CONFIG_TABLE = "postgres";

    void fillConfig(Config& config, const toml::table& table)
    {
        const auto& logConfigNode = table[LOG_CONFIG_TABLE];
        if (logConfigNode)
        {
            config.log = common::parseLogConfig(logConfigNode);
        }

        const auto& postgresConfigNode = table[POSTGRES_CONFIG_TABLE];
        if (postgresConfigNode)
        {
            config.postgres = common::parsePostgresConfig(postgresConfigNode);
        }

        // TODO: either postgres or redis config must be present
        if (!config.postgres)
        {
            throw std::invalid_argument("Config file has no storage config table");
        }
    }
}

bool parseConfig(std::string_view filePath, Config& config)
{
    try
    {
        const auto tomlTable = toml::parse_file(filePath);
        fillConfig(config, tomlTable);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}
