#include <common/utils/log.hpp>
#include <producer/postgres_producer.hpp>

#include <sstream>

namespace
{
    const std::string MESSAGES_TABLE = "messages";
}

PostgresProducer::PostgresProducer(const common::PostgresConfig& postgresConfig, const ProducerConfig& producerConfig)
    : connectionString_(postgresConfig.connectionString())
    , chunkSize_(producerConfig.chunkSize)
{
}

Messages PostgresProducer::getNext()
{
    auto& connection = getConnection();
    auto work = pqxx::work{connection};

    const auto query = "SELECT id, data FROM " + MESSAGES_TABLE + " ORDER BY id ASC LIMIT ($1)";
    const auto rows = work.exec_params(query, chunkSize_);

    Messages result;
    for (const auto& row : rows)
    {
        auto id = row[0].as<decltype(Message::id)>();
        auto data = pqxx::binarystring(row[1]);
        result.push_back(Message{
            .id = id,
            .data = {data.data(), data.data() + data.size()}
        });
    }
    return result;
}

void PostgresProducer::markConsumed(const Messages& messages)
{
    auto& connection = getConnection();
    auto work = pqxx::work{connection};

    std::ostringstream query;
    query << "DELETE FROM " << MESSAGES_TABLE << " WHERE id IN (";
    for (size_t i = 0; i < messages.size() - 1; ++i)
    {
        query << messages[i].id << ',';
    }
    query << messages.back().id << ')';

    work.exec(query.str());
    work.commit();
}

pqxx::connection& PostgresProducer::getConnection()
{
    if (!connection_)
    {
        connection_ = std::make_unique<pqxx::connection>(connectionString_);
        Log(debug) << "Connection to Postgres established";
    }
    return *connection_;
}
