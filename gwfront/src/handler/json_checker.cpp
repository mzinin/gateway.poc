#include <common/utils/log.hpp>
#include <handler/json_checker.hpp>

#include <nlohmann/json.hpp>

#include <sstream>
#include <string_view>
#include <vector>

namespace
{
    // TODO replace with real fields
    const std::vector<std::string_view> MANDATORY_FIELDS = {
        "field1", "field2", "field3"
    };
}

JsonChecker::Result JsonChecker::check(const std::string& data)
{
    // false - i.e. no exceptions
    const auto json = nlohmann::json::parse(data, nullptr, false);
    if (json.is_discarded())
    {
        Log(error) << "JsonChecker: data is not a valid json";
        return {false, "data is not a valid json"};
    }

    for (const auto& mandatoryField : MANDATORY_FIELDS)
    {
        const auto it = json.find(mandatoryField);
        if (it == json.end())
        {
            std::ostringstream message;
            message << "field '" << mandatoryField << "' is missing";
            Log(error) << "JsonChecker: " << message.view();
            return {false, std::move(message).str()};
        }

        if (!it->is_string())
        {
            std::ostringstream message;
            message << "field '" << mandatoryField << "' is missing";
            Log(error) << "JsonChecker: " << message.view();
            return {false, std::move(message).str()};
        }
    }

    return {true, ""};
}
