#include <handler/postgres_writer.hpp>

#include <pqxx/pqxx>

#include <sstream>

namespace
{
    std::string configToConnectionString(const PostgresConfig& config)
    {
        std::ostringstream buffer;
        buffer << "host=" << config.host << " "
               << "port=" << config.port << " "
               << "dbname=" << config.database << " "
               << "user=" << config.user << " "
               << "password=" << config.password;
        return buffer.str();
    }
}

PostgresWriter::PostgresWriter(const PostgresConfig& config)
    : connectionString_(configToConnectionString(config))
{
}

PostgresWriter::Result PostgresWriter::write(const std::string& data)
{
    try
    {
        auto connection = pqxx::connection{connectionString_};
        auto work = pqxx::work{connection};

        work.exec_params("INSERT INTO messages (data) VALUES ($1)",
                         pqxx::binarystring(data.data(), data.size()));
        work.commit();
        return {Error::OK, ""};
    }
    catch (const pqxx::broken_connection& e)
    {
        return {Error::STORAGE_UNAVAILABLE, e.what()};
    }
    catch (const std::exception& e)
    {
        return {Error::GENERIC, e.what()};
    }
}
