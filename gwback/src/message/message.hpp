#pragma once

#include <cstdint>
#include <vector>


struct Message final
{
    uint64_t id;
    std::vector<uint8_t> data;
};

using Messages = std::vector<Message>;
