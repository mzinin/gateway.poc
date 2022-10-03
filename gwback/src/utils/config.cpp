#include <utils/config.hpp>

#include <toml++/toml.h>

#include <iostream>
#include <stdexcept>

namespace
{
    constexpr std::string_view CONSUMER_CONFIG_TABLE = "consumer";
    constexpr std::string_view LOG_CONFIG_TABLE = "log";
    constexpr std::string_view POSTGRES_CONFIG_TABLE = "postgres";
    constexpr std::string_view PRODUCER_CONFIG_TABLE = "producer";
    constexpr std::string_view REDIS_CONFIG_TABLE = "redis";

    constexpr std::string_view CHUNK_SIZE_PARAMETER = "chunk_size";
    constexpr std::string_view MESSAGE_LIMIT_PARAMETER = "message_limit";
    constexpr std::string_view OUTPUT_FOLDER_PARAMETER = "output_folder";

    constexpr int64_t MIN_CHUNK = 1;
    constexpr int64_t MAX_CHUNK = 1000000;

    ConsumerConfig parseConsumerConfig(const toml::node_view<const toml::node>& node)
    {
        ConsumerConfig result;

        // parse and check chunk value
        const auto chunkSize = node[CHUNK_SIZE_PARAMETER].as_integer();
        if (chunkSize)
        {
            const auto chunkSizeValue = chunkSize->get();
            if (chunkSizeValue < MIN_CHUNK || MAX_CHUNK < chunkSizeValue)
            {
                throw std::invalid_argument("Consumer config has wrong chunk size value: " + std::to_string(chunkSizeValue));
            }
            result.chunkSize = static_cast<decltype(ConsumerConfig::chunkSize)>(chunkSizeValue);
        }

        // parse and check output folder value
        const auto outputFolder = node[OUTPUT_FOLDER_PARAMETER].as_string();
        if (!outputFolder || outputFolder->get().empty())
        {
            throw std::invalid_argument("Consumer config has no proper output folder parameter");
        }
        result.outputFolder = outputFolder->get();

        return result;
    }

    ProducerConfig parseProducerConfig(const toml::node_view<const toml::node>& node)
    {
        ProducerConfig result;

        // parse and check chunk value
        const auto chunkSize = node[CHUNK_SIZE_PARAMETER].as_integer();
        if (chunkSize)
        {
            const auto chunkSizeValue = chunkSize->get();
            if (chunkSizeValue < MIN_CHUNK || MAX_CHUNK < chunkSizeValue)
            {
                throw std::invalid_argument("Consumer config has wrong chunk size value: " + std::to_string(chunkSizeValue));
            }
            result.chunkSize = static_cast<decltype(ProducerConfig::chunkSize)>(chunkSizeValue);
        }

        // parse message limit value
        const auto messageLimit = node[MESSAGE_LIMIT_PARAMETER].as_integer();
        if (messageLimit)
        {
            const auto messageLimitValue = messageLimit->get();
            result.messageLimit = static_cast<decltype(ProducerConfig::chunkSize)>(messageLimitValue);
        }

        return result;
    }

    void fillConfig(Config& config, const toml::table& table)
    {
        const auto& logConfigNode = table[LOG_CONFIG_TABLE];
        if (logConfigNode)
        {
            config.log = common::parseLogConfig(logConfigNode);
        }

        const auto& postgresConfigNode = table[POSTGRES_CONFIG_TABLE];
        if (postgresConfigNode)
        {
            config.postgres = common::parsePostgresConfig(postgresConfigNode);
        }

        const auto& redisConfigNode = table[REDIS_CONFIG_TABLE];
        if (redisConfigNode)
        {
            config.redis = common::parseRedisConfig(redisConfigNode);
        }

        if (!config.postgres && !config.redis)
        {
            throw std::invalid_argument("Config file has no storage config table");
        }

        const auto& consumerConfigNode = table[CONSUMER_CONFIG_TABLE];
        if (!consumerConfigNode)
        {
            throw std::invalid_argument("Config file has no consumer config table");
        }
        config.consumer = parseConsumerConfig(consumerConfigNode);

        const auto& producerConfigNode = table[PRODUCER_CONFIG_TABLE];
        if (!producerConfigNode)
        {
            throw std::invalid_argument("Config file has no producer config table");
        }
        config.producer = parseProducerConfig(producerConfigNode);
    }
}

bool parseConfig(std::string_view filePath, Config& config)
{
    try
    {
        const auto tomlTable = toml::parse_file(filePath);
        fillConfig(config, tomlTable);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
}
