#include <utils/config.hpp>
#include <utils/log.hpp>

#include <boost/program_options.hpp>

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
}

int main(int argc, char* argv[])
{
    if (!parseArguments(argc, argv))
    {
        return EXIT_FAILURE;
    }

    Config config;
    try
    {
        parseConfig(configPath, config);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initLog(config.log);
    Log(info) << "Starting Gateway ...";

    Log(trace) << "Trace";
    Log(debug) << "Debug";
    Log(info) << "Info";
    Log(warning) << "Warning";
    Log(error) << "Error";
    Log(fatal) << "Fatal";

    return EXIT_SUCCESS;
}
