#include <common/utils/log.hpp>
#include <handler/postgres_writer.hpp>

#include <algorithm>
#include <mutex>


namespace
{
    const std::string MESSAGES_TABLE = "messages";
}

PostgresWriter::PostgresWriter(const ConfigType& config)
    : connectionString_(config.connectionString())
{
}

PostgresWriter::Result PostgresWriter::write(const std::string& data)
{
    bool doReconnect = true;
    while (true)
    {
        try
        {
            auto& connection = findConnection();
            auto executor = pqxx::nontransaction{connection};

            const auto query = "INSERT INTO " + MESSAGES_TABLE + " (data) VALUES ($1)";
            executor.exec_params(query, pqxx::binarystring(data.data(), data.size()));
            executor.commit();

            return {Error::OK, ""};
        }
        catch (const pqxx::broken_connection& e)
        {
            if (doReconnect)
            {
                reconnect();
                doReconnect = false;
            }
            else
            {
                return {Error::STORAGE_UNAVAILABLE, e.what()};
            }
        }
        catch (const std::exception& e)
        {
            return {Error::GENERIC, e.what()};
        }
    }
}

pqxx::connection& PostgresWriter::findConnection()
{
    // try to find existing connection
    {
        std::shared_lock lock(mutex_);
        auto it = std::find_if(connections_.begin(),
                               connections_.end(),
                               [threadId = std::this_thread::get_id()](const auto& ec){ return ec.threadId == threadId; });
        if (it != connections_.end())
        {
            return *it->connection;
        }
    }

    // create a new connection
    {
        std::unique_lock lock(mutex_);
        connections_.push_back({
            .threadId = std::this_thread::get_id(),
            .connection = std::make_unique<pqxx::connection>(connectionString_)
        });
        Log(debug) << "Connection to Postgres established";
        return *connections_.back().connection;
    }
}

void PostgresWriter::reconnect()
{
    std::unique_lock lock(mutex_);

    auto it = std::find_if(connections_.begin(),
                           connections_.end(),
                           [threadId = std::this_thread::get_id()](const auto& ec){ return ec.threadId == threadId; });

    if (it != connections_.end())
    {
        it->connection = std::make_unique<pqxx::connection>(connectionString_);
        Log(debug) << "Connection to Postgres reestablished";
    }
}
