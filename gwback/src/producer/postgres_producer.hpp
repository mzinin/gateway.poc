#pragma once

#include <common/utils/config.hpp>
#include <producer/interface.hpp>

#include <pqxx/pqxx>

#include <memory>
#include <string>


class PostgresProducer final : public IProducer
{
public:
    PostgresProducer(const common::PostgresConfig& config);

    Messages getNext(uint32_t limit) override;
    void markConsumed(const Messages& messages) override;

private:
    pqxx::connection& getConnection();

private:
    const std::string connectionString_;
    std::unique_ptr<pqxx::connection> connection_;
};
