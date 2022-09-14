#include <common/utils/config.hpp>

#include <set>
#include <stdexcept>
#include <sstream>
#include <string_view>


namespace
{
    constexpr std::string_view DATABASE_PARAMETER = "database";
    constexpr std::string_view HOST_PARAMETER = "host";
    constexpr std::string_view PASSWORD_PARAMETER = "password";
    constexpr std::string_view PORT_PARAMETER = "port";
    constexpr std::string_view SEVERITY_PARAMETER = "severity";
    constexpr std::string_view TYPE_PARAMETER = "type";
    constexpr std::string_view USER_PARAMETER = "user";

    const std::set<std::string> SEVERITIES = {"trace", "debug", "info", "warning", "error", "fatal"};

    const std::string JSON_MESSAGE_TYPE = "json";
    const std::string FLATBUFFERS_MESSAGE_TYPE = "flatbuffers";

    constexpr int64_t MIN_PORT = 1;
    constexpr int64_t MAX_PORT = 65535;
}

std::string common::PostgresConfig::connectionString() const
{
    std::ostringstream buffer;
    buffer << "host=" << host << " "
            << "port=" << port << " "
            << "dbname=" << database << " "
            << "user=" << user << " "
            << "password=" << password;
    return buffer.str();
}

common::LogConfig common::parseLogConfig(const toml::node_view<const toml::node>& node)
{
    LogConfig result;

    const auto severity = node[SEVERITY_PARAMETER].as_string();
    if (severity)
    {
        const auto severityValue = severity->get();
        if (SEVERITIES.count(severityValue) == 0)
        {
            throw std::invalid_argument("Log config has no proper severity parameter");
        }
        result.severity = severityValue;
    }

    return result;
}

common::MessagesConfig common::parseMessagesConfig(const toml::node_view<const toml::node>& node)
{
    MessagesConfig result;

    const auto type = node[TYPE_PARAMETER].as_string();
    if (!type || type->get().empty())
    {
        throw std::invalid_argument("Messages config has no proper type parameter");
    }

    const auto typeValue = type->get();
    if (typeValue == JSON_MESSAGE_TYPE)
    {
        result.type = MessagesConfig::Type::JSON;
    }
    else if (typeValue == FLATBUFFERS_MESSAGE_TYPE)
    {
        result.type = MessagesConfig::Type::FLATBUFFERS;
    }
    else
    {
        throw std::invalid_argument("Messages config has wrong type value: " + typeValue);
    }

    return result;
}

common::PostgresConfig common::parsePostgresConfig(const toml::node_view<const toml::node>& node)
{
    PostgresConfig result;

    // parse and check host value
    const auto host = node[HOST_PARAMETER].as_string();
    if (!host || host->get().empty())
    {
        throw std::invalid_argument("Postgres config has no proper host parameter");
    }
    result.host = host->get();

    // parse and check port value
    const auto port = node[PORT_PARAMETER].as_integer();
    if (port)
    {
        const auto portValue = port->get();
        if (portValue < MIN_PORT || MAX_PORT < portValue)
        {
            throw std::invalid_argument("Postgres config has wrong port value: " + std::to_string(portValue));
        }
        result.port = static_cast<decltype(PostgresConfig::port)>(portValue);
    }

    // parse and check database value
    const auto database = node[DATABASE_PARAMETER].as_string();
    if (!database || database->get().empty())
    {
        throw std::invalid_argument("Postgres config has no proper database parameter");
    }
    result.database = database->get();

    // parse and check user value
    const auto user = node[USER_PARAMETER].as_string();
    if (!user || user->get().empty())
    {
        throw std::invalid_argument("Postgres config has no proper user parameter");
    }
    result.user = user->get();

    // parse and check password value
    const auto password = node[PASSWORD_PARAMETER].as_string();
    if (!password)
    {
        throw std::invalid_argument("Postgres config has no proper password parameter");
    }
    result.password = password->get();

    return result;
}

common::RedisConfig common::parseRedisConfig(const toml::node_view<const toml::node>& node)
{
    RedisConfig result;

    // parse and check host value
    const auto host = node[HOST_PARAMETER].as_string();
    if (!host || host->get().empty())
    {
        throw std::invalid_argument("Redis config has no proper host parameter");
    }
    result.host = host->get();

    // parse and check port value
    const auto port = node[PORT_PARAMETER].as_integer();
    if (port)
    {
        const auto portValue = port->get();
        if (portValue < MIN_PORT || MAX_PORT < portValue)
        {
            throw std::invalid_argument("Redis config has wrong port value: " + std::to_string(portValue));
        }
        result.port = static_cast<decltype(PostgresConfig::port)>(portValue);
    }

    // parse and check user value
    const auto user = node[USER_PARAMETER].as_string();
    if (!user || user->get().empty())
    {
        throw std::invalid_argument("Redis config has no proper user parameter");
    }
    result.user = user->get();

    // parse and check password value
    const auto password = node[PASSWORD_PARAMETER].as_string();
    if (!password)
    {
        throw std::invalid_argument("Redis config has no proper password parameter");
    }
    result.password = password->get();

    return result;
}
