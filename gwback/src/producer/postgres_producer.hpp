#pragma once

#include <producer/interface.hpp>
#include <utils/config.hpp>

#include <pqxx/pqxx>

#include <memory>
#include <string>


class PostgresProducer final : public IProducer
{
public:
    PostgresProducer(const common::PostgresConfig& postgresConfig, const ProducerConfig& producerConfig);

    Messages getNext() override;
    void markConsumed(const Messages& messages) override;

private:
    pqxx::connection& getConnection();

private:
    const std::string connectionString_;
    const uint32_t chunkSize_;
    std::unique_ptr<pqxx::connection> connection_;
};
