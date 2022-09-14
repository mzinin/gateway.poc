#include <common/messages/instrument.hpp>
#include <common/utils/log.hpp>
#include <handler/flatbuffers_checker.hpp>

#include <cstdint>


FlatbuffersChecker::Result FlatbuffersChecker::check(const std::string& data)
{
    auto verifier = flatbuffers::Verifier(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    if (!messages::VerifyInstrumentBuffer(verifier))
    {
        Log(error) << "FlatbuffersChecker: data is not a valid flatbuffer instrument";
        return {false, "data is not a valid flatbuffer instrument"};
    }

    const auto* instrument = messages::GetInstrument(data.data());

    {
        const auto* str = instrument->str0();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str0' is missing";
            return {false, "field 'str0' is missing"};
        }
    }

    {
        const auto* str = instrument->str1();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str1' is missing";
            return {false, "field 'str1' is missing"};
        }
    }

    {
        const auto* str = instrument->str2();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str2' is missing";
            return {false, "field 'str2' is missing"};
        }
    }

    {
        const auto* str = instrument->str3();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str3' is missing";
            return {false, "field 'str3' is missing"};
        }
    }

    {
        const auto* str = instrument->str4();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str4' is missing";
            return {false, "field 'str4' is missing"};
        }
    }

    {
        const auto* str = instrument->str5();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str5' is missing";
            return {false, "field 'str5' is missing"};
        }
    }

    {
        const auto* str = instrument->str6();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str6' is missing";
            return {false, "field 'str6' is missing"};
        }
    }

    {
        const auto* str = instrument->str7();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str7' is missing";
            return {false, "field 'str7' is missing"};
        }
    }

    {
        const auto* str = instrument->str8();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str8' is missing";
            return {false, "field 'str8' is missing"};
        }
    }

    {
        const auto* str = instrument->str9();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str9' is missing";
            return {false, "field 'str9' is missing"};
        }
    }

    {
        const auto* str = instrument->str10();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str10' is missing";
            return {false, "field 'str10' is missing"};
        }
    }

    {
        const auto* str = instrument->str11();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str11' is missing";
            return {false, "field 'str11' is missing"};
        }
    }

    {
        const auto* str = instrument->str12();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str12' is missing";
            return {false, "field 'str12' is missing"};
        }
    }

    {
        const auto* str = instrument->str13();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str13' is missing";
            return {false, "field 'str13' is missing"};
        }
    }

    {
        const auto* str = instrument->str14();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str14' is missing";
            return {false, "field 'str14' is missing"};
        }
    }

    {
        const auto* str = instrument->str15();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str15' is missing";
            return {false, "field 'str15' is missing"};
        }
    }

    {
        const auto* str = instrument->str16();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str16' is missing";
            return {false, "field 'str16' is missing"};
        }
    }

    {
        const auto* str = instrument->str17();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str17' is missing";
            return {false, "field 'str17' is missing"};
        }
    }

    {
        const auto* str = instrument->str18();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str18' is missing";
            return {false, "field 'str18' is missing"};
        }
    }

    {
        const auto* str = instrument->str19();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str19' is missing";
            return {false, "field 'str19' is missing"};
        }
    }

    {
        const auto* str = instrument->str20();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str20' is missing";
            return {false, "field 'str20' is missing"};
        }
    }

    {
        const auto* str = instrument->str21();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str21' is missing";
            return {false, "field 'str21' is missing"};
        }
    }

    {
        const auto* str = instrument->str22();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str22' is missing";
            return {false, "field 'str22' is missing"};
        }
    }

    {
        const auto* str = instrument->str23();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str23' is missing";
            return {false, "field 'str23' is missing"};
        }
    }

    {
        const auto* str = instrument->str24();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str24' is missing";
            return {false, "field 'str24' is missing"};
        }
    }

    {
        const auto* str = instrument->str25();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str25' is missing";
            return {false, "field 'str25' is missing"};
        }
    }

    {
        const auto* str = instrument->str26();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str26' is missing";
            return {false, "field 'str26' is missing"};
        }
    }

    {
        const auto* str = instrument->str27();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str27' is missing";
            return {false, "field 'str27' is missing"};
        }
    }

    {
        const auto* str = instrument->str28();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str28' is missing";
            return {false, "field 'str28' is missing"};
        }
    }

    {
        const auto* str = instrument->str29();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str29' is missing";
            return {false, "field 'str29' is missing"};
        }
    }

    {
        const auto* str = instrument->str30();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str30' is missing";
            return {false, "field 'str30' is missing"};
        }
    }

    {
        const auto* str = instrument->str31();
        if (!str || str->size() == 0)
        {
            Log(error) << "FlatbuffersChecker: field 'str31' is missing";
            return {false, "field 'str31' is missing"};
        }
    }

    return {true, ""};
}
