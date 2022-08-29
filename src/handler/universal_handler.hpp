#pragma once

#include <handler/interface.hpp>

template <class Checker>
class UniversalHandler final : public IHandler
{
public:
    Result operator()(const std::string& data) override
    {
        auto checkResult = Checker::check(data);
        if (!checkResult.ok)
        {
            return {Error::BAD_DATA, std::move(checkResult.error)};
        }

        return {Error::OK, "OK"};
    }
};
