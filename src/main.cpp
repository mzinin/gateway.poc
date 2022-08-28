#include <handler/interface.hpp>
#include <http/server.hpp>
#include <utils/config.hpp>
#include <utils/log.hpp>

#include <boost/program_options.hpp>

#include <csignal>
#include <cstdlib>
#include <iostream>

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

    void waitSignal()
    {
        sigset_t signalSet;
        sigemptyset(&signalSet);

        sigaddset(&signalSet, SIGINT);
        sigaddset(&signalSet, SIGTERM);
        sigaddset(&signalSet, SIGQUIT);

        sigprocmask(SIG_BLOCK, &signalSet, nullptr);

        int signal = 0;
        sigwait(&signalSet, &signal);

        Log(info) << "Got signal " << signal;
    }
}

class DummyHandler final : public IHandler
{
public:
    Result operator()(const std::string&) override
    {
        return {Error::OK, "OK"};
    }
};

int main(int argc, char* argv[])
{
    Config config;
    if (!parseArguments(argc, argv) || !parseConfig(configPath, config))
    {
        return EXIT_FAILURE;
    }

    initLog(config.log);
    Log(info) << "Starting gateway";

    DummyHandler handler{};

    try
    {
        HttpServer server{config.http, handler};
        server.start();

        waitSignal();

        server.stop();
    }
    catch (const std::exception& e)
    {
        Log(error) << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
