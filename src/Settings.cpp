#include "Settings.h"

#include <algorithm>
#include <unordered_map>

Settings::Settings(int argc, char *argv[], std::vector<Particle> &particles) : particles(particles) {
  parse_result = this->parseArguments(argc, argv);
}

void Settings::printHelp() {
  std::cout
      << "Usage: ./MolSim\n\n"
         "Simulates Molecules. For detailed Description see README.md\n\n"
         "Options:\n"
         "-w, --week=UINT          select which week's simulation to run (1=PlanetSimulation, 2=Collision Simulation "
         "(Default))\n"
         "-s, --single=FILE        reads single particles from the file in xyz format\n"
         "-c, --cuboid=FILE        reads particles from the file in cuboid format\n"
         "-c, --cuboid=FILE        reads particles and simulation parameters from a yaml file\n"
         "-o, --out=FILE           path and name of the output files. Path has to exist! (default: MD_vtk)\n"
         "-e, --t_end=DOUBLE       sets t_end (default 1000)\n"
         "-d, --delta_t=DOUBLE     sets delta_t (default 0.014)\n"
         "-b, --BrownMotionMean    sets the mean for the Brown motion\n"
         "-l, --logLevel=STRING    sets the Level of logging. (off, error, warning, info (default), debug, trace)\n"
         "-h, --help               shows this Text end terminates the program\n\n"
         "Example:\n"
         "./MolSim -e 100.0 -y ../input/assignment3.yaml -o ../out/out"
      << std::endl;
}

Settings::PARSE_RESULT Settings::parseArguments(int argc, char *argv[]) {
  const char *const short_opts = "e:d:w:s:c:y:b:ho:l:";
  const option long_opts[] = {{"t_end", required_argument, nullptr, 'e'},
                              {"delta_t", required_argument, nullptr, 'd'},
                              {"week", required_argument, nullptr, 'w'},
                              {"single", required_argument, nullptr, 's'},
                              {"cuboid", required_argument, nullptr, 'c'},
                              {"yaml", required_argument, nullptr, 'y'},
                              {"BrownMotionMean", required_argument, nullptr, 'b'},
                              {"help", no_argument, nullptr, 'h'},
                              {"out", required_argument, nullptr, 'o'},
                              {"logLevel", required_argument, nullptr, 'l'},
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
          YAMLReader::parse(particles, optarg, *this);
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

        case 'l': {
          std::string logLevelText(optarg);
          spdlog::level::level_enum logLevel = stringToLogLevel(logLevelText);
          spdlog::set_level(logLevel);
          break;
        }
        case 'h':
          return HELP;
        case '?':
          spdlog::error("Unknown option: {}", static_cast<char>(optopt));
        default:
          spdlog::error("An error occurred while passing the arguments.");
          return ERROR;
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
      {"info", spdlog::level::info}, {"warn", spdlog::level::warn},   {"err", spdlog::level::err},
  };

  auto gato = lookup.find(string);
  if (gato == lookup.end()) {
    spdlog::error("Loglevel does not exist");
    return spdlog::level::info;
  }

  return gato->second;
}

void Settings::createOutputDirectory(std::filesystem::path directory) {
  if (std::filesystem::exists(directory)) return;

  spdlog::warn("Output directory {} does not exist, creating it", directory.string());
  std::filesystem::create_directories(directory);
}
