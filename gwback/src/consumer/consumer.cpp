#include <common/utils/log.hpp>
#include <consumer/consumer.hpp>

#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <thread>


namespace
{
    void saveMessages(const Messages& messages, const std::string& folder)
    {
        if (messages.empty())
        {
            return;
        }

        auto minId = std::numeric_limits<decltype(Message::id)>::max();
        auto maxId = std::numeric_limits<decltype(Message::id)>::min();
        for (const auto& message : messages)
        {
            minId = std::min(minId, message.id);
            maxId = std::max(minId, message.id);
        }

        const auto fileName = std::to_string(minId) + "-" + std::to_string(maxId);
        const auto fullPath = std::filesystem::path(folder) / fileName;

        std::ofstream output{fullPath, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc};
        if (!output.good())
        {
            Log(error) << "Failed to open file '" << fullPath << "' for writing";
            return;
        }

        for (const auto& message : messages)
        {
            output.write(reinterpret_cast<const char*>(message.data.data()), message.data.size());
            output << "\n";
        }
    }
}

MessageConsumer::MessageConsumer(const ConsumerConfig& config)
    : chunkSize_(config.chunkSize)
    , outputFolder_(config.outputFolder)
{
    buffer_.reserve(chunkSize_);
}

MessageConsumer::~MessageConsumer()
{
    saveMessages(buffer_, outputFolder_);
}

bool MessageConsumer::operator()(Messages& messages)
{
    for (auto& message : messages)
    {
        buffer_.emplace_back(std::move(message));
        if (buffer_.size() >= chunkSize_)
        {
            flushBuffer();
        }
    }
    return true;
}

void MessageConsumer::flushBuffer()
{
    std::thread(saveMessages, std::move(buffer_), outputFolder_).detach();
}
