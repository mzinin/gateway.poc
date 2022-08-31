#include <utils/config.hpp>

#include <tomlplusplus/toml.hpp>

#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace
{
    constexpr std::string_view HTTP_CONFIG_TABLE = "http";
    constexpr std::string_view LOG_CONFIG_TABLE = "log";
    constexpr std::string_view POSTGRES_CONFIG_TABLE = "postgres";

    constexpr std::string_view DATABASE_PARAMETER = "database";
    constexpr std::string_view HOST_PARAMETER = "host";
    constexpr std::string_view PASSWORD_PARAMETER = "password";
    constexpr std::string_view PORT_PARAMETER = "port";
    constexpr std::string_view SEVERITY_PARAMETER = "severity";
    constexpr std::string_view REQUEST_TIMEOUT_PARAMETER = "request_timeout";
    constexpr std::string_view THREADS_PARAMETER = "threads";
    constexpr std::string_view USER_PARAMETER = "user";

    const std::set<std::string> SEVERITIES = {"trace", "debug", "info", "warning", "error", "fatal"};

    constexpr int64_t MIN_PORT = 1;
    constexpr int64_t MAX_PORT = 65535;
    constexpr int64_t MIN_THREADS = 1;
    constexpr int64_t MAX_THREADS = 256;
    constexpr int64_t MIN_TIMEOUT = 1;
    constexpr int64_t MAX_TIMEOUT = 60;

    LogConfig parseLogConfig(const toml::node_view<const toml::node>& node)
    {
        const auto severity = node[SEVERITY_PARAMETER].as_string();
        if (!severity)
        {
            return {};
        }
        const auto severityValue = severity->get();
        if (SEVERITIES.count(severityValue) == 0)
        {
            throw std::invalid_argument("Log config has no proper severity parameter");
        }

        return {.severity = severityValue};
    }

    HttpServerConfig parseHttpServerConfig(const toml::node_view<const toml::node>& node)
    {
        HttpServerConfig result;

        // parse and check port value
        const auto port = node[PORT_PARAMETER].as_integer();
        if (!port)
        {
            throw std::invalid_argument("Http server config has no proper port parameter");
        }
        const auto portValue = port->get();
        if (portValue < MIN_PORT || MAX_PORT < portValue)
        {
            throw std::invalid_argument("Http server config has wrong port value: " + std::to_string(portValue));
        }
        result.port = static_cast<decltype(HttpServerConfig::port)>(portValue);

        // parse and check threads value
        const auto threads = node[THREADS_PARAMETER].as_integer();
        if (!threads)
        {
            throw std::invalid_argument("Http server config has no proper threads parameter");
        }
        const auto threadsValue = threads->get();
        if (threadsValue < MIN_THREADS || MAX_THREADS < threadsValue)
        {
            throw std::invalid_argument("Http server config has wrong threads value: " + std::to_string(threadsValue));
        }
        result.threads = static_cast<decltype(HttpServerConfig::threads)>(threadsValue);

        // parse and check request timeout
        const auto timeout = node[REQUEST_TIMEOUT_PARAMETER].as_integer();
        if (timeout)
        {
            const auto timeoutValue = timeout->get();
            if (timeoutValue < MIN_TIMEOUT || MAX_TIMEOUT < timeoutValue)
            {
                throw std::invalid_argument("Http server config has wrong request timeout value: " + std::to_string(timeoutValue));
            }
            result.requestTimeout = static_cast<decltype(HttpServerConfig::requestTimeout)>(timeoutValue);
        }

        return result;
    }

    PostgresConfig parsePostgresConfig(const toml::node_view<const toml::node>& node)
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

    void fillConfig(Config& config, const toml::table& table)
    {
        const auto& logConfigNode = table[LOG_CONFIG_TABLE];
        if (logConfigNode)
        {
            config.log = parseLogConfig(logConfigNode);
        }

        const auto& httpServerConfigNode = table[HTTP_CONFIG_TABLE];
        if (!httpServerConfigNode)
        {
            throw std::invalid_argument("Config file has no http server config table");
        }
        config.http = parseHttpServerConfig(httpServerConfigNode);

        const auto& postgresConfigNode = table[POSTGRES_CONFIG_TABLE];
        if (postgresConfigNode)
        {
            config.postgres = parsePostgresConfig(postgresConfigNode);
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
