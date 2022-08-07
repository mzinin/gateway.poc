#include <utils/config.hpp>

#include <tomlplusplus/toml.hpp>

#include <limits>


namespace
{
    constexpr std::string_view HTTP_CONFIG_TABLE = "http";
    constexpr std::string_view PORT_PARAMETER = "port";
    constexpr std::string_view THREADS_PARAMETER = "threads";

    constexpr int64_t MIN_PORT = 1;
    constexpr int64_t MAX_PORT = 65535;
    constexpr int64_t MIN_THREADS = 1;
    constexpr int64_t MAX_THREADS = 256;

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

Config::Config(std::string_view filePath)
{
    const auto tomlTable = toml::parse_file(filePath);

    const auto& httpServerConfigNode = tomlTable[HTTP_CONFIG_TABLE];
    if (!httpServerConfigNode)
    {
        throw std::invalid_argument("Config file has no http server config table");
    }
    http = parseHttpServerConfig(httpServerConfigNode);
}
