#pragma once

#include <common/utils/config.hpp>

#include <boost/log/trivial.hpp>


namespace common
{
    void initLog(const LogConfig& config);

} // namespace common

#define Log(severity) BOOST_LOG_TRIVIAL(severity)
