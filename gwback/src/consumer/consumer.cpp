#include <common/utils/log.hpp>
#include <consumer/consumer.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
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

        std::ostringstream fileName;
        fileName << std::setw(19) << std::setfill('0') << minId;
        fileName << '-';
        fileName << std::setw(19) << std::setfill('0') << maxId;
        const auto fullPath = std::filesystem::path(folder) / fileName.str();

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

        Log(info) << messages.size() << " messages saved to '" << fullPath << "'";
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
