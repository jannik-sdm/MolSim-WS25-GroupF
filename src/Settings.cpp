#include "Settings.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks-inl.h>
#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <unordered_map>

#include "inputReader/CuboidReader.h"
#include "inputReader/FileReader.h"
#include "inputReader/XYZReader.h"
#include "inputReader/YAMLReader.h"

void Settings::printHelp() {
  std::cout << "Simulates Molecules. For a detailed description see README.md\n\n"
               "Usage:\n"
               "  -o, --out=FILE                  Path and name of the output files\n"
               "  -f, --frequency=UINT            Frequency at which output files are written\n"
               "  -w, --worksheet=UINT            Select which worksheets's simulation to run\n"
               "  -e, --end-time=DOUBLE           Sets the end time\n"
               "  -d, --delta-t=DOUBLE            Sets the timestep\n"
               "  -b, --brown-motion-avg=DOUBLE   Sets the average mean for the brownian motion\n"
               "  -s, --single=FILE               Reads particles from the specified file in xyz format\n"
               "  -c, --cuboid=FILE               Reads particles from the specified file in cuboid format\n"
               "  -y, --yaml=FILE                 Reads particles and settings from the specified file in yaml format\n"
               "  -l, --loglevel=STRING           Set the log level (trace, debug, info, warn, error)\n"
               "  -h, --help                      Show this help text and terminates the program.\n\n"
               "Example:\n"
               "  MolSim -e 100.0 -c ../input/eingabe-cuboid.txt"
            << std::endl;
}

void Settings::parseArguments(int argc, char *argv[]) {
  const char *const short_opts = "e:d:w:s:c:y:b:ho:f:l:";
  const option long_opts[] = {{"end-time", required_argument, nullptr, 'e'},
                              {"delta-t", required_argument, nullptr, 'd'},
                              {"worksheet", required_argument, nullptr, 'w'},
                              {"single", required_argument, nullptr, 's'},
                              {"cuboid", required_argument, nullptr, 'c'},
                              {"yaml", required_argument, nullptr, 'y'},
                              {"brown-motion-avg", required_argument, nullptr, 'b'},
                              {"help", no_argument, nullptr, 'h'},
                              {"out", required_argument, nullptr, 'o'},
                              {"frequency", required_argument, nullptr, 'f'},
                              {"loglevel", required_argument, nullptr, 'l'},
                              {nullptr, no_argument, nullptr, 0}};

  while (true) {
    try {
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

      if (-1 == opt) break;

      // getopt schreibt evtl noch etwas auf stderr.
      switch (opt) {
        case 'e':
          simulation.end_time = std::stod(optarg);
          break;

        case 'd':
          simulation.delta_t = std::stod(optarg);
          break;

        case 'w':
          simulation.worksheet = std::stoul(optarg);
          break;

        case 's':
          FileReader<XYZReader>::readFile(particles, optarg);
          break;

        case 'c':
          FileReader<CuboidReader>::readFile(particles, optarg);
          break;

        case 'y':
          try {
            YAMLReader::readFile(particles, optarg, *this);
          } catch (YAML::Exception) {
            spdlog::error("Error parsing YAML file, aborting");
            exit(EXIT_FAILURE);
          }
          break;

        case 'b':
          simulation.brown_motion_avg_velocity = std::stod(optarg);
          break;

        case 'o':
          output.directory = optarg;
          createOutputDirectory(optarg);
          break;

        case 'f':
          output.frequency = std::stoul(optarg);
          break;

        case 'l':
          output.log_level = spdlog::level::from_str(optarg);
          break;

        case 'h':
          printHelp();
          exit(EXIT_SUCCESS);
          break;

        case '?':
          spdlog::error("Unknown option: -{}", static_cast<char>(optopt));
          [[fallthrough]];
        default:
          spdlog::error("An error occurred while passing the arguments.");
          exit(EXIT_FAILURE);
          break;
      }
    } catch (const std::invalid_argument &e) {
      spdlog::error("Could not parse arguments! Is the Type of the Arguments correct?");
      exit(EXIT_FAILURE);
    }
  }
}

void Settings::createOutputDirectory(std::filesystem::path directory) {
  if (std::filesystem::exists(directory)) return;

  spdlog::warn("Output directory {} does not exist, creating it", directory.string());
  std::filesystem::create_directories(directory);
}

/**
 * @brief Initialize spdlog
 *
 * Sets some default options and enables async logging for spdlog
 * @see https://github.com/gabime/spdlog
 */
void initializeLogging() {
  spdlog::init_thread_pool(8192, 1);
  // Create Sinks
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/log.txt", true);  // true: overrides already existing files instad of appending
  // Create Logger
  std::vector<spdlog::sink_ptr> sinks{stdout_sink, file_sink};
  auto async_logger = std::make_shared<spdlog::async_logger>(
      "async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  // Set Defaults
  spdlog::set_default_logger(async_logger);

  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
}
