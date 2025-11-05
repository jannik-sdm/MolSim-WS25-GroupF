/**
 * @file MolSim.cpp
 *
 */
#include <iostream>
#include <list>
#include <getopt.h>

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
            << "brown_motion_mean = " << brown_motion_mean << std::endl;



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
    "-e, --t_end=DOUBLE     sets t_end (default 1000)\n"
    "-d, --delta_t=DOUBLE   sets delta_t (default 0.014)\n"
    "-b, --BrownMotionMean  sets the mean for the Brown motion\n"
    "-h, --help             shows this Text end terminates the program\n\n"
    "Example:\n"
    "./MolSim -e 100.0 -f ../input/eingabe-sonne.txt"<<std::endl;
}

  int parseArgs(int argc, char *argv[]) {

    const char* const short_opts = "e:d:f:b:h";
    const option long_opts[] = {
      {"t_end", required_argument, nullptr, 'e'},
      {"delta_t", required_argument, nullptr, 'd'},
      {"file", required_argument, nullptr, 'f'},
      {"BrownMotionMean", required_argument, nullptr, 'b'},
      {"help", no_argument, nullptr, 'h'},
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
          fileReader.readFile(particleContainer.particles, optarg);
          break;
        }

        case 'b':
          brown_motion_mean = std::stod(optarg);
          std::cout << "brown_motion_mean set to:" << std::endl;
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
  std::string out_name("MD_vtk");

#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particleContainer.particles, out_name, iteration);
}
