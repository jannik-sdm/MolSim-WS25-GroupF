#include "Settings.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <unordered_map>

Settings::Settings(int argc, char *argv[], std::vector<Particle> &particles) : particles(particles) {
  parse_result = parseArguments(argc, argv);
}

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

Settings::PARSE_RESULT Settings::parseArguments(int argc, char *argv[]) {
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
          end_time = std::stod(optarg);
          spdlog::debug("endtime set to: {}", end_time);
          break;

        case 'd':
          delta_t = std::stod(optarg);
          spdlog::debug("delta_t set to: {}", delta_t);
          break;

        case 'w':
          worksheet = std::stoul(optarg);
          spdlog::debug("week set to: {}", worksheet);
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
            return ERROR;
          }
          break;

        case 'b':
          brown_motion_avg_velocity = std::stod(optarg);
          spdlog::debug("brown_motion_mean set to: {}", brown_motion_avg_velocity);
          break;

        case 'o':
          outputFolder = optarg;
          createOutputDirectory(optarg);
          spdlog::debug("output folder set to: {}", outputFolder.string());
          break;

        case 'f':
          frequency = std::stoul(optarg);
          spdlog::debug("frequency set to: {}", frequency);
          break;

        case 'l': {
          spdlog::level::level_enum logLevel = stringToLogLevel(optarg);
          spdlog::set_level(logLevel);
        } break;

        case 'h':
          return HELP;
          break;

        case '?':
          spdlog::error("Unknown option: {}", static_cast<char>(optopt));
        default:
          spdlog::error("An error occurred while passing the arguments.");
          return ERROR;
          break;
      }
    } catch (const std::invalid_argument &e) {
      spdlog::error("Error: could not parse arguments!");
      spdlog::error("Is the Type of the Arguments correct?");
      return ERROR;
    }
  }

  return SUCCESS;
}

spdlog::level::level_enum Settings::stringToLogLevel(std::string string) {
  // transform string to lower case
  std::transform(string.begin(), string.end(), string.begin(), ::tolower);

  const std::unordered_map<std::string, spdlog::level::level_enum> lookup = {
      {"off", spdlog::level::off},   {"trace", spdlog::level::trace}, {"debug", spdlog::level::debug},
      {"info", spdlog::level::info}, {"warn", spdlog::level::warn},   {"error", spdlog::level::err},
  };

  auto level = lookup.find(string);
  if (level == lookup.end()) {
    spdlog::error("Loglevel does not exist");
    return spdlog::level::info;
  }

  return level->second;
}

void Settings::createOutputDirectory(std::filesystem::path directory) {
  if (std::filesystem::exists(directory)) return;

  spdlog::warn("Output directory {} does not exist, creating it", directory.string());
  std::filesystem::create_directories(directory);
}
