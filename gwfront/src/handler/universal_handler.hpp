#pragma once

#include <common/utils/log.hpp>
#include <handler/interface.hpp>


template <class Checker, class Writer>
class UniversalHandler final : public IHandler
{
public:
    UniversalHandler(const Writer::ConfigType& config)
        : writer_(config)
    {
    }

    Result operator()(const std::string& data) override
    {
        auto checkResult = Checker::check(data);
        if (!checkResult.ok)
        {
            Log(error) << "UniversalHandler: bad data, " << checkResult.message;
            return {Error::BAD_DATA, std::move(checkResult.message)};
        }

        auto writeResult = writer_.write(data);
        switch (writeResult.error)
        {
            case Writer::Error::GENERIC:
                Log(error) << "UniversalHandler: generic error, " << writeResult.message;
                return {Error::GENERIC, std::move(writeResult.message)};

            case Writer::Error::STORAGE_UNAVAILABLE:
            Log(error) << "UniversalHandler: storage unavailable, " << writeResult.message;
                return {Error::STORAGE_UNAVAILABLE, std::move(writeResult.message)};

            default:
                break;
        }

        return {Error::OK, ""};
    }

private:
    Writer writer_;
};
