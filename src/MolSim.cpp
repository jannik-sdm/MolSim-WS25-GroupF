/**
 * @file MolSim.cpp
 *
 */
#include <iostream>
#include <list>
#include <getopt.h>
#include "inputReader/FileReader.h"
#include "inputReader/CuboidReader.h"
#include "ParticleContainer.h"
#include "PlanetSimulation.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"

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
double end_time = 1000;
double delta_t = 0.014;
double brown_motion_mean = 1.1264; //Some default Value. Please set this value right later?
std::string out_name("MD_vtk");

ParticleContainer particleContainer;

int main(int argc, char *argsv[]) {
  if (parseArgs(argc, argsv) != 0 || particleContainer.particles.empty()){
    printHelp();
    return -1;
  }
  // use given parameters, or default endtime = 1000 delta_t = 0.014
  std::cout << "Parsed Arguments\n Starting simulation with parameters:" << std::endl
            << "endtime = " << end_time << std::endl
            << "delta_t = " << delta_t << std::endl
            << "brown_motion_mean = " << brown_motion_mean << std::endl
            << "output path/name = " << out_name << std::endl;



  // select simulation
  PlanetSimulation simulation = PlanetSimulation(particleContainer, end_time, delta_t);
  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {

    simulation.iteration();
    iteration++;

    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}

void printHelp() {
  std::cout <<
    "Usage: ./MolSim\n\n"
    "Simulates Molecules. For detailed Description see README.md\n\n"
    "Options:\n"
    "-f, --file=FILE        reads particles from the file\n"
    "-o, --out=FILE         path and name of the output files. Path has to exist! (default: MD_vtk)\n"
    "-e, --t_end=DOUBLE     sets t_end (default 1000)\n"
    "-d, --delta_t=DOUBLE   sets delta_t (default 0.014)\n"
    "-b, --BrownMotionMean  sets the mean for the Brown motion\n"
    "-h, --help             shows this Text end terminates the program\n\n"
    "Example:\n"
    "./MolSim -e 100.0 -f ../input/eingabe-sonne.txt"<<std::endl;
}

 // Source: https://gist.github.com/ashwin/d88184923c7161d368a9
  int parseArgs(int argc, char *argv[]) {

    const char* const short_opts = "e:d:f:b:ho:";
    const option long_opts[] = {
      {"t_end", required_argument, nullptr, 'e'},
      {"delta_t", required_argument, nullptr, 'd'},
      {"file", required_argument, nullptr, 'f'},
      {"BrownMotionMean", required_argument, nullptr, 'b'},
      {"help", no_argument, nullptr, 'h'},
      {"out", required_argument, nullptr, 'o'},
      {nullptr, no_argument, nullptr, 0}
    };

    while (true)
    {
      try{
      const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

      if (-1 == opt)
        break;

      switch (opt) {
        case 'e':
          end_time = std::stod(optarg);
          std::cout << "endtime set to: " << end_time << std::endl;
          break;

        case 'd':
          delta_t = std::stod(optarg);
          std::cout << "delta_t set to: " << delta_t << std::endl;
          break;

        case 'f': {
          FileReader fileReader;
          std::cout << "Reading File" << std::endl;
  FileReader<CuboidReader>::readFile(particleContainer.particles, optarg);
          break;
        }

        case 'b':
          brown_motion_mean = std::stod(optarg);
          std::cout << "brown_motion_mean set to:" << std::endl;
          break;
        case 'o':
          out_name = optarg;
          break;
        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
          return -1;
      }
      }catch(const std::invalid_argument& e) {
        std::cout << "Error: could not parse arguments!\n"
                     "Is the Type of the Arguments correct?" << std::endl;
        std::cout << e.what() << "\n \n";
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
