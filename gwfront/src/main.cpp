#include <common/utils/log.hpp>
#include <common/utils/wait_signal.hpp>
#include <handler/interface.hpp>
#include <handler/json_checker.hpp>
#include <handler/postgres_writer.hpp>
#include <handler/universal_handler.hpp>
#include <http/server.hpp>
#include <utils/config.hpp>

#include <boost/program_options.hpp>

#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{
    std::string configPath;

    bool parseArguments(int argc, char* argv[])
    {
        namespace po = boost::program_options;

        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "print this message")
            ("config,c", po::value<std::string>(&configPath)->required(), "path to config file");

        po::variables_map vm;

        try
        {
            po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

            if (vm.count("help"))
            {
                std::cout << "Usage:\n\t" << argv[0] << " [options]" << std::endl;
                std::cout << desc;
                return false;
            }

            po::notify(vm);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << desc;
            return false;
        }

        return true;
    }
}

int main(int argc, char* argv[])
{
    Config config;
    if (!parseArguments(argc, argv) || !parseConfig(configPath, config))
    {
        return EXIT_FAILURE;
    }

    common::initLog(config.log);
    Log(info) << "Starting gateway front";

    try
    {
        std::unique_ptr<IHandler> handler;
        if (config.postgres)
        {
            handler = std::make_unique<UniversalHandler<JsonChecker, PostgresWriter>>(*config.postgres);
        }

        HttpServer server{config.http, *handler};
        server.start();
        Log(info) << "Gateway front started";

        common::waitSignal();

        Log(info) << "Stopping gateway front";
        server.stop();
        Log(info) << "Gateway front stopped";
    }
    catch (const std::exception& e)
    {
        Log(error) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
