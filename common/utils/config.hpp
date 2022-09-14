#pragma once

#include <toml++/toml.h>

#include <cstdint>
#include <string>


namespace common
{
    struct LogConfig final
    {
        std::string severity = "error";
    };

    struct MessagesConfig final
    {
        enum class Type
        {
            JSON,
            FLATBUFFERS
        };

        Type type;
    };

    struct PostgresConfig final
    {
        std::string host;
        uint16_t port = 5432;
        std::string database;
        std::string user;
        std::string password;

        std::string connectionString() const;
    };

    struct RedisConfig final
    {
        std::string host;
        uint16_t port = 6379;
        std::string user;
        std::string password;
    };

    LogConfig parseLogConfig(const toml::node_view<const toml::node>& node);
    MessagesConfig parseMessagesConfig(const toml::node_view<const toml::node>& node);
    PostgresConfig parsePostgresConfig(const toml::node_view<const toml::node>& node);
    RedisConfig parseRedisConfig(const toml::node_view<const toml::node>& node);

} // namespace common
