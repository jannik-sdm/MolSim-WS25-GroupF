/**
 * @file MolSim.cpp
 *
 */
#include <iostream>
#include <list>
#include <getopt.h>

#include "spdlog/spdlog.h"
#include "FileReader.h"
#include "ParticleContainer.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 *@brief Parses the Arguments
 *@tparam argc number of commandline arguments
 *@tparam argv[] commandline arguments
 *@return 0 successful, -1 otherwise
 */
int parseArgs(int argc, char *argv[]);

/**
 * @brief calculate the force for all particles
 *
 * For each pair of disjunct particles this function calculates the force f with the Formula: \f$ F_{ij} =
 * \frac{m_im_j}{(||x_i-x_j||_2)^3}(x_j-x_i)\f$.
 * Then this function sums up all forces for one particle to calculate the effective force of each particle
 */
void calculateF();

/**
 * @brief calculate the position for all particles
 *
 * For each particle i this function calculates the position x: \f$ x_i(t_{n+1}) = x_i(t_n)+\Delta t \cdot v_i(t_n) +
 * (\Delta t)^2 \frac{F_i(t_n)}{2m_i}\f$
 */
void calculateX();

/**
 * @brief calculate the Velocity for all particles
 *
 * For each particle i this function calculates the Velocity v: \f$ v_i(t_{n+1}) = v_i(t_n)+\Delta t
 * \frac{F_i(t_n)+F_i(t_{n+1})}{2m_i}\f$
 */
void calculateV();

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
double end_time = 1000;
double delta_t = 0.014;
double brown_motion_mean = 1.1264; //Some default Value. Please set this value right later?
std::string out_name("MD_vtk");

ParticleContainer particleContainer;

int main(int argc, char *argsv[]) {
  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
  spdlog::set_level(spdlog::level::info); //set default
  if (parseArgs(argc, argsv) != 0 || particleContainer.particles.empty()){
    printHelp();
    return -1;
  }
  // use given parameters, or default endtime = 1000 delta_t = 0.014
  spdlog::info("Parsed Arguments:");
  spdlog::info("Starting simulation with parameters:");
  spdlog::info("endtime = {}",end_time);
  spdlog::info("delta_t = {}", delta_t);
  spdlog::info("brown_motion_mean = {}", brown_motion_mean);
  spdlog::info("output path/name = {}",out_name);



  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();

    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }
    spdlog::trace("Iteration {} finished.", iteration);

    current_time += delta_t;
  }

  spdlog::info("output written. Terminating...");
  return 0;
}

void printHelp() {
  std::cout <<
    "Usage: ./MolSim\n\n"
    "Simulates Molecules. For detailed Description see README.md\n\n"
    "Options:\n"
    "-f, --file=FILE          reads particles from the file\n"
    "-o, --out=FILE           path and name of the output files. Path has to exist! (default: MD_vtk)\n"
    "-e, --t_end=DOUBLE       sets t_end (default 1000)\n"
    "-d, --delta_t=DOUBLE     sets delta_t (default 0.014)\n"
    "-b, --BrownMotionMean    sets the mean for the Brown motion\n"
    "-l, --logLevel=LOGLEVEL  sets the Level of logging. (ERROR, WARNING, INFO (default), DEBUG, TRACE)\n"
    "-h, --help               shows this Text end terminates the program\n\n"
    "Example:\n"
    "./MolSim -e 100.0 -f ../input/eingabe-sonne.txt"<<std::endl;
}

 // Source: https://gist.github.com/ashwin/d88184923c7161d368a9
  int parseArgs(int argc, char *argv[]) {

    const char* const short_opts = "e:d:f:b:ho:l:";
    const option long_opts[] = {
      {"t_end", required_argument, nullptr, 'e'},
      {"delta_t", required_argument, nullptr, 'd'},
      {"file", required_argument, nullptr, 'f'},
      {"BrownMotionMean", required_argument, nullptr, 'b'},
      {"help", no_argument, nullptr, 'h'},
      {"out", required_argument, nullptr, 'o'},
      {"logLevel", required_argument, nullptr, 'l'},
      {nullptr, no_argument, nullptr, 0}
    };

    while (true)
    {
      try{
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

      if (-1 == opt)
        break;

        //getopt schreibt evtl noch etwas auf stderr.
      switch (opt) {
        case 'e':
          end_time = std::stod(optarg);
          spdlog::debug("endtime set to: {}", end_time);
          break;

        case 'd':
          delta_t = std::stod(optarg);
          spdlog::debug("delta_t set to: {}",delta_t);
          break;

        case 'f': {
          FileReader fileReader;
          fileReader.readFile(particleContainer.particles, optarg);
          break;
        }

        case 'b':
          brown_motion_mean = std::stod(optarg);
          spdlog::debug("brown_motion_mean set to: {}", brown_motion_mean);
          break;
        case 'o':
          out_name = optarg;
          break;
        case 'l': {
          std::string logLevel(optarg);
          //kein switch-case, weil das nur auf integern geht
          if (logLevel == "ERROR") {
            spdlog::set_level(spdlog::level::err);
          }else if ( logLevel == "WARN") {
            spdlog::set_level(spdlog::level::warn);
          }else if ( logLevel == "INFO") {
            spdlog::set_level(spdlog::level::info);
          }else if ( logLevel == "DEBUG") {
            spdlog::set_level(spdlog::level::debug);
          }else if (logLevel == "TRACE") {
            spdlog::set_level(spdlog::level::trace);
          }else {
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
      }catch(const std::invalid_argument& e) {
        spdlog::error("Error: could not parse arguments!");
        spdlog::error("Is the Type of the Arguments correct?");
        return -1;
      }
    }
  return 0;
  }

void calculateF() {
  for (auto &p : particleContainer) p.setF({0, 0, 0});
  for (auto it = particleContainer.pairs_begin(); it != particleContainer.pairs_end(); ++it) {
    auto [p1, p2] = *it;

    const double a = 1 / pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 3);
    auto f = a * p1.getM() * p2.getM() * (p2.getX() - p1.getX());

    p1.addF(f);
    p2.subF(f);
  }
}

void calculateX() {
  for (auto &p : particleContainer) {
    // @TODO: insert calculation of position updates here!
    const double a = 1 / (2 * p.getM());
    p.setX(p.getX() + delta_t * p.getV() + pow(delta_t, 2) * a * p.getF());
  }
}

void calculateV() {
  for (auto &p : particleContainer) {
    // @TODO: insert calculation of velocity updates here!
    const double a = 1 / (2 * p.getM());
    p.setV(p.getV() + delta_t * a * (p.getOldF() + p.getF()));
  }
}

void plotParticles(int iteration) {


#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particleContainer.particles, out_name, iteration);
}
