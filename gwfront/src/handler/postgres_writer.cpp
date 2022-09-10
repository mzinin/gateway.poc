#include <common/utils/log.hpp>
#include <handler/postgres_writer.hpp>

#include <algorithm>
#include <mutex>


namespace
{
    const std::string MESSAGES_TABLE = "messages";
    const int RECONNECT_RETRIES = 2;
}

PostgresWriter::PostgresWriter(const ConfigType& config)
    : connectionString_(config.connectionString())
{
}

PostgresWriter::Result PostgresWriter::write(const std::string& data)
{
    int retriesLeft = RECONNECT_RETRIES;
    while (true)
    {
        try
        {
            auto& connection = findConnection();
            auto work = pqxx::work{connection};

            const auto query = "INSERT INTO " + MESSAGES_TABLE + " (data) VALUES ($1)";
            work.exec_params(query, pqxx::binarystring(data.data(), data.size()));
            work.commit();

            return {Error::OK, ""};
        }
        catch (const pqxx::broken_connection& e)
        {
            if (retriesLeft != 0)
            {
                --retriesLeft;
                continue;
            }
            return {Error::STORAGE_UNAVAILABLE, e.what()};
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
