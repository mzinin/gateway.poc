#include <common/utils/log.hpp>
#include <common/utils/wait_signal.hpp>
#include <consumer/consumer.hpp>
#include <producer/postgres_producer.hpp>
#include <utils/config.hpp>

#include <boost/program_options.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

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

    constexpr std::chrono::duration PAUSE = std::chrono::seconds(1);
    std::atomic_bool run = true;

    void runLoop(IProducer& producer, MessageConsumer& consumer)
    {
        while (run)
        {
            try
            {
                auto messages = producer.getNext();
                Log(trace) << "Get " << messages.size() << " messages from producer";

                if (messages.empty())
                {
                    std::this_thread::sleep_for(PAUSE);
                    continue;
                }

                if (consumer(messages))
                {
                    producer.markConsumed(messages);
                }
            }
            catch (const std::exception& e)
            {
                Log(error) << "Erro while processing messages: " << e.what();
                std::this_thread::sleep_for(PAUSE);
            }
        }
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
        auto producer = std::make_unique<PostgresProducer>(*config.postgres, config.producer);
        auto consumer = MessageConsumer{config.consumer};

        auto thread = std::thread(runLoop, std::ref(*producer), std::ref(consumer));
        Log(info) << "Gateway back started";

        common::waitSignal();

        Log(info) << "Stopping gateway back";
        run = false;
        thread.join();
        Log(info) << "Gateway back stopped";
    }
    catch (const std::exception& e)
    {
        Log(error) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
