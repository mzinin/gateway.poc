#include <producer/postgres_producer.hpp>

PostgresProducer::PostgresProducer(const common::PostgresConfig& config)
    : connectionString_(config.connectionString())
{
}

Messages PostgresProducer::getNext(uint32_t limit)
{
    auto& connection = getConnection();
    auto work = pqxx::work{connection};

    const auto rows = work.exec_params("SELECT id, data FROM messages ORDER BY id ASC LIMIT ($1)", limit);

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

void PostgresProducer::markConsumed(const Messages& /*messages*/)
{
    // TODO
}

pqxx::connection& PostgresProducer::getConnection()
{
    if (!connection_)
    {
        connection_ = std::make_unique<pqxx::connection>(connectionString_);
    }
    return *connection_;
}
