#pragma once

#include <string>

class IHandler
{
public:
    enum class Error
    {
        OK,
        GENERIC,
        BAD_DATA,
        STORAGE_UNAVAILABLE,
    };

    struct Result
    {
        Error error;
        std::string message;
    };

public:
    virtual ~IHandler() = default;
    virtual Result operator()(const std::string& data) = 0;
};
