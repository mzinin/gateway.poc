#include <common/utils/log.hpp>
#include <handler/json_checker.hpp>

#include <nlohmann/json.hpp>

#include <optional>
#include <sstream>
#include <string_view>
#include <vector>

namespace
{
    std::optional<JsonChecker::Result> checkIntegerField(const nlohmann::json& json, std::string_view field)
    {
        const auto it = json.find(field);
        if (it == json.end())
        {
            std::ostringstream message;
            message << "field '" << field << "' is missing";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        if (!it->is_number_integer())
        {
            std::ostringstream message;
            message << "field '" << field << "' is not an integer";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        return {};
    }

    std::optional<JsonChecker::Result> checkDoubleField(const nlohmann::json& json, std::string_view field)
    {
        const auto it = json.find(field);
        if (it == json.end())
        {
            std::ostringstream message;
            message << "field '" << field << "' is missing";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        if (!it->is_number_float() && !it->is_number_integer())
        {
            std::ostringstream message;
            message << "field '" << field << "' is not a double";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        return {};
    }

    std::optional<JsonChecker::Result> checkStringField(const nlohmann::json& json, std::string_view field)
    {
        const auto it = json.find(field);
        if (it == json.end())
        {
            std::ostringstream message;
            message << "field '" << field << "' is missing";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        if (!it->is_string())
        {
            std::ostringstream message;
            message << "field '" << field << "' is not a string";
            return JsonChecker::Result{false, std::move(message).str()};
        }

        return {};
    }
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

    for (int i = 0; i < 6; ++i)
    {
        const auto result = checkIntegerField(json, "int" + std::to_string(i));
        if (result)
        {
            Log(error) << "JsonChecker: " << result->message;
            return *result;
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        const auto result = checkDoubleField(json, "double" + std::to_string(i));
        if (result)
        {
            Log(error) << "JsonChecker: " << result->message;
            return *result;
        }
    }

    for (int i = 0; i < 32; ++i)
    {
        const auto result = checkStringField(json, "str" + std::to_string(i));
        if (result)
        {
            Log(error) << "JsonChecker: " << result->message;
            return *result;
        }
    }

    return {true, ""};
}
