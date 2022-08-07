#pragma once

#include <utils/config.hpp>

#include <boost/log/trivial.hpp>

#include <string_view>


#define Log(severity) BOOST_LOG_TRIVIAL(severity)

void initLog(const LogConfig& config);
