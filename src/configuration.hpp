#include <filesystem>

#include "../libs/argparse.hpp"
#include "../libs/dotenv.h"

#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
namespace maxmodels
{
    struct Configuration
    {
        bool isDebug = false;
        bool useMapping = false;
        bool printRules = false;
        bool printSimplifiedRules = false;
        bool printLabels = false;
        bool printWeights = false;
        bool printClauses = false;
        bool printSolverOutput = false;
        bool forceOpenWBO = false;
        std::string openWBOPath;
        std::string wMaxCDCLPath;
        unsigned int solverIterationLimit = 1000000;
    };

    Configuration getConfiguration(int argc, char *argv[])
    {
        Configuration configuration;

        ap::parser p(argc, argv);
        p.add("-e", "--env", "Configuration as a .env file.", ap::mode::REQUIRED);
        p.add("-d", "--debug", "Do not perform searching.", ap::mode::BOOLEAN);
        p.add("-m", "--use-mapping", "Map the printed data.", ap::mode::BOOLEAN);
        p.add("-r", "--print-rules", "Print the rules.", ap::mode::BOOLEAN);
        p.add("-s", "--print-simplified-rules", "Print the simplified rules.", ap::mode::BOOLEAN);
        p.add("-l", "--print-labels", "Print the labels.", ap::mode::BOOLEAN);
        p.add("-w", "--print-weights", "Print the weights.", ap::mode::BOOLEAN);
        p.add("-c", "--print-wcnf", "Print the program in the WCNF format.", ap::mode::BOOLEAN);
        p.add("-o", "--print-solver-output", "Print the solver's output.", ap::mode::BOOLEAN);
        p.add("-f", "--force-open-wbo", "Use Open-WBO even if the weights are provided.", ap::mode::BOOLEAN);

        auto args = p.parse();
        if (!args.parsed_successfully())
        {
            throw std::runtime_error("The command line arguments cannot be parsed. Use `--help` for more information.");
        }
        configuration.isDebug = (bool)stoi(args["-d"]);
        configuration.useMapping = (bool)stoi(args["-m"]);
        configuration.printRules = (bool)stoi(args["-r"]);
        configuration.printSimplifiedRules = (bool)stoi(args["-s"]);
        configuration.printLabels = (bool)stoi(args["-l"]);
        configuration.printWeights = (bool)stoi(args["-w"]);
        configuration.printClauses = (bool)stoi(args["-c"]);
        configuration.printSolverOutput = (bool)stoi(args["-o"]);
        configuration.forceOpenWBO = (bool)stoi(args["-f"]);

        std::string dotEnvFilePath = args["-e"];
        if (std::filesystem::exists(dotEnvFilePath) == false)
        {
            throw std::runtime_error("The path to the configuration does not exist.");
        }

        dotenv::init(dotEnvFilePath.c_str());

        const char *openWBOPath = getenv("OPENWBO");
        if (openWBOPath == nullptr || std::string(openWBOPath).empty())
        {
            throw std::runtime_error("Open-WBO is not set or empty in the environment variables.");
        }
        configuration.openWBOPath = openWBOPath;

        const char *wMaxCDCLPath = getenv("WMAXCDCL");
        if (wMaxCDCLPath == nullptr || std::string(wMaxCDCLPath).empty())
        {
            throw std::runtime_error("WMaxCDCL is not set or empty in the environment variables.");
        }
        configuration.wMaxCDCLPath = wMaxCDCLPath;

        return configuration;
    }
}
#endif
