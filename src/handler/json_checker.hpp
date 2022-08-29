#pragma once

#include <string>


class JsonChecker final
{
public:
    struct Result
    {
        bool ok;
        std::string error;
    };

    static Result check(const std::string& data);
};
