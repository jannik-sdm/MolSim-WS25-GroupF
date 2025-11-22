/**
 * @file MolSim.cpp
 *
 */
#include <getopt.h>
#include <iostream>
#include <chrono>
#include <spdlog/sinks/stdout_color_sinks-inl.h>

#include "ParticleContainer.h"
#include "inputReader/CuboidReader.h"
#include "inputReader/FileReader.h"
#include "inputReader/XYZReader.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "simulations/CollisionSimulation.h"
#include "simulations/PlanetSimulation.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"


/**** forward declaration of the calculation functions ****/

/**
 *@brief Parses the Arguments
 *@tparam argc number of commandline arguments
 *@tparam argv[] commandline arguments
 *@return 0 successful, -1 otherwise
 */
int parseArgs(int argc, char *argv[]);

/**
 * @brief plot the particles to a xyz-file or to a vtk-file.
 *
 * If ENABLE_VTK_OUTPUT is set, this function creates a vtk-file. Otherwise it creates a xyz-file
 */
void plotParticles(int iteration);

/**
 * @brief Prints Help Message to the Commandline
 *
 */
void printHelp();

constexpr double start_time = 0;
double end_time = 5;
double delta_t = 0.0002;
double brown_motion_mean = 0.1;
std::string out_name("MD_vtk");

unsigned int week = 2;

ParticleContainer particleContainer;

int main(int argc, char *argsv[]) {

  //https://github.com/gabime/spdlog
  
  spdlog::init_thread_pool(8192, 1);
  //Create Sinks
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.txt",true /*überschreibt File, falls schon existend*/);
  // Create Logger
  std::vector<spdlog::sink_ptr> sinks {stdout_sink, file_sink};
  auto async_logger = std::make_shared<spdlog::async_logger>("async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  //Set Defaults
  spdlog::set_default_logger(async_logger);
  
  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
  spdlog::set_level(spdlog::level::info);  // set default
  if (parseArgs(argc, argsv) != 0 || particleContainer.particles.empty()) {
    printHelp();
    return -1;
  }
  // use given parameters, or default endtime = 1000 delta_t = 0.014
  spdlog::info("Parsed Arguments:");
  spdlog::info("Starting simulation with parameters:");
  spdlog::info("endtime = {}", end_time);
  spdlog::info("delta_t = {}", delta_t);
  spdlog::info("brown_motion_mean = {}", brown_motion_mean);
  spdlog::info("output path/name = {}", out_name);
#ifdef ENABLE_TIME_MEASURE
  // Source for duration measurement- https://stackoverflow.com/a/19312610
  auto start_time_measure = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ENABLE_TIME_MEASURE; i++) {
#endif

    // select simulation
    std::unique_ptr<Simulation> simulation = nullptr;

    switch (week) {
      case 1:
        simulation = std::make_unique<PlanetSimulation>(particleContainer, end_time, delta_t);
        break;

      case 2:
      default:
        simulation = std::make_unique<CollisionSimulation>(particleContainer, end_time, delta_t);
        break;
    };

    double current_time = start_time;

    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
      simulation->iteration();
      iteration++;

      if (iteration % 10 == 0) {
        plotParticles(iteration);
      }
      spdlog::trace("Iteration {} finished.", iteration);

      current_time += delta_t;
    }
#ifdef ENABLE_TIME_MEASURE
  }
    spdlog::info("output written. Terminating...");
    auto end_time_measure = std::chrono::high_resolution_clock::now();

    spdlog::info("Program has been running for {} ms", std::chrono::duration_cast<std::chrono::milliseconds>((end_time_measure - start_time_measure)/ENABLE_TIME_MEASURE).count());
#endif

  return 0;
}

void printHelp() {
  std::cout << "Usage: ./MolSim\n\n"
               "Simulates Molecules. For detailed Description see README.md\n\n"
               "Options:\n"
               "-w, --week=UINT          select which week's simulation to run (1=PlanetSimulation, 2=Collision Simulation (Default))\n"
               "-s, --single=FILE        reads single particles from the file in xyz format\n"
               "-c, --cuboid=FILE        reads particles from the file in cuboid format\n"
               "-o, --out=FILE           path and name of the output files. Path has to exist! (default: MD_vtk)\n"
               "-e, --t_end=DOUBLE       sets t_end (default 1000)\n"
               "-d, --delta_t=DOUBLE     sets delta_t (default 0.014)\n"
               "-b, --BrownMotionMean    sets the mean for the Brown motion\n"
               "-l, --logLevel=STRING    sets the Level of logging. (OFF, ERROR, WARNING, INFO (default), DEBUG, TRACE)\n"
               "-h, --help               shows this Text end terminates the program\n\n"
               "Example:\n"
               "./MolSim -e 100.0 -p ../input/eingabe-cuboid.txt"
            << std::endl;
}

// Source: https://gist.github.com/ashwin/d88184923c7161d368a9
int parseArgs(int argc, char *argv[]) {
  const char *const short_opts = "e:d:w:s:c:b:ho:l:";
  const option long_opts[] = {
      {"t_end", required_argument, nullptr, 'e'},    {"delta_t", required_argument, nullptr, 'd'},
      {"week", required_argument, nullptr, 'w'},     {"single", required_argument, nullptr, 's'},
      {"cuboid", required_argument, nullptr, 'c'},   {"BrownMotionMean", required_argument, nullptr, 'b'},
      {"help", no_argument, nullptr, 'h'},           {"out", required_argument, nullptr, 'o'},
      {"logLevel", required_argument, nullptr, 'l'}, {nullptr, no_argument, nullptr, 0}};

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
          week = std::stoul(optarg);
          spdlog::debug("week set to: {}", week);
          break;

        case 's':
          FileReader<XYZReader>::readFile(particleContainer.particles, optarg);
          break;

        case 'c':
          FileReader<CuboidReader>::readFile(particleContainer.particles, optarg);
          break;

        case 'b':
          brown_motion_mean = std::stod(optarg);
          spdlog::debug("brown_motion_mean set to: {}", brown_motion_mean);
          break;
        case 'o':
          out_name = optarg;
          break;
        case 'l': {
          std::string logLevel(optarg);
          // kein switch-case, weil das nur auf integern geht
          if (logLevel == "OFF") {
            spdlog::set_level(spdlog::level::off);
          }else if (logLevel == "ERROR") {
            spdlog::set_level(spdlog::level::err);
          } else if (logLevel == "WARN") {
            spdlog::set_level(spdlog::level::warn);
          } else if (logLevel == "INFO") {
            spdlog::set_level(spdlog::level::info);
          } else if (logLevel == "DEBUG") {
            spdlog::set_level(spdlog::level::debug);
          } else if (logLevel == "TRACE") {
            spdlog::set_level(spdlog::level::trace);
          } else {
            spdlog::error("Typo in logLevel? This LogLevel doesn't exist");
            return -1;
          }
          break;
        }
        case 'h':
          return -1;
        case '?':
          spdlog::error("Unknown option: {}", static_cast<char>(optopt));
          return -1;
        default:
          spdlog::error("An error occurred while passing the arguments.");
          return -1;
      }
    } catch (const std::invalid_argument &e) {
      spdlog::error("Error: could not parse arguments!");
      spdlog::error("Is the Type of the Arguments correct?");
      return -1;
    }
  }
  return 0;
}

void plotParticles(int iteration) {
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particleContainer.particles, out_name, iteration);
}
