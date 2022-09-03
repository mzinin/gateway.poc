#pragma once

#include <tomlplusplus/toml.hpp>

#include <cstdint>
#include <string>


namespace common
{
    struct LogConfig final
    {
        std::string severity = "error";
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

    LogConfig parseLogConfig(const toml::node_view<const toml::node>& node);
    PostgresConfig parsePostgresConfig(const toml::node_view<const toml::node>& node);

} // namespace common
