#include <utils/config.hpp>

#include <tomlplusplus/toml.hpp>

#include <limits>
#include <set>

namespace
{
    constexpr std::string_view HTTP_CONFIG_TABLE = "http";
    constexpr std::string_view LOG_CONFIG_TABLE = "log";

    constexpr std::string_view PORT_PARAMETER = "port";
    constexpr std::string_view SEVERITY_PARAMETER = "severity";
    constexpr std::string_view THREADS_PARAMETER = "threads";

    const std::set<std::string> SEVERITIES = {"trace", "debug", "info", "warning", "error", "fatal"};

    constexpr int64_t MIN_PORT = 1;
    constexpr int64_t MAX_PORT = 65535;
    constexpr int64_t MIN_THREADS = 1;
    constexpr int64_t MAX_THREADS = 256;

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

        return HttpServerConfig{
            .port = static_cast<decltype(HttpServerConfig::port)>(portValue),
            .threads = static_cast<decltype(HttpServerConfig::threads)>(threadsValue)
        };
    }
}

void parseConfig(std::string_view filePath, Config& config)
{
    const auto tomlTable = toml::parse_file(filePath);

    const auto& logConfigNode = tomlTable[LOG_CONFIG_TABLE];
    if (logConfigNode)
    {
        config.log = parseLogConfig(logConfigNode);
    }

    const auto& httpServerConfigNode = tomlTable[HTTP_CONFIG_TABLE];
    if (!httpServerConfigNode)
    {
        throw std::invalid_argument("Config file has no http server config table");
    }
    config.http = parseHttpServerConfig(httpServerConfigNode);
}
