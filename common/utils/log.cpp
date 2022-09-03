#include <common/utils/log.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <cstdint>
#include <iomanip>
#include <map>
#include <optional>

namespace
{
    const std::map<std::string_view, boost::log::trivial::severity_level> severityMap = {
        {"trace", boost::log::trivial::trace},
        {"debug", boost::log::trivial::debug},
        {"info", boost::log::trivial::info},
        {"warning", boost::log::trivial::warning},
        {"error", boost::log::trivial::error},
        {"fatal", boost::log::trivial::fatal}
    };

    std::optional<boost::log::trivial::severity_level> stringToBoostSeverity(std::string_view severity)
    {
        const auto it = severityMap.find(severity);
        return it == severityMap.end() ? std::nullopt : std::optional{it->second};
    }
}

void common::initLog(const LogConfig& config)
{
    const auto boostSeverity = stringToBoostSeverity(config.severity);
    if (!boostSeverity)
    {
        Log(warning) << "initLog: unknown severity '" << config.severity << "'";
    }
    else
    {
        boost::log::core::get()->set_filter
        (
            boost::log::trivial::severity >= *boostSeverity
        );
    }

    boost::log::add_console_log
    (
        std::cout,
        boost::log::keywords::format =
        (
            boost::log::expressions::stream
                << "[" << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "] "
                << "[" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "] "
                << "[" << std::left << std::setw(7) << std::setfill(' ') << boost::log::trivial::severity << "] "
                << boost::log::expressions::smessage
        )
    );

    boost::log::add_common_attributes();
}
