#include <common/utils/log.hpp>
#include <common/utils/wait_signal.hpp>
#include <consumer/consumer.hpp>
#include <producer/postgres_producer.hpp>
#include <utils/config.hpp>

#include <boost/program_options.hpp>

#include <csignal>
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
    Log(info) << "Starting gateway back";

    try
    {
        auto producer = PostgresProducer{*config.postgres, config.producer};
        auto consumer = MessageConsumer{config.consumer};

        auto msgs = producer.getNext();
        consumer(msgs);

        Log(info) << "number of messages: " << msgs.size();
        for (const auto& msg : msgs)
        {
            Log(info) << "id: " << msg.id << ", data: " << std::string_view{reinterpret_cast<const char*>(msg.data.data()), msg.data.size()};
        }

        producer.markConsumed(msgs);

        common::waitSignal();
    }
    catch (const std::exception& e)
    {
        Log(error) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}