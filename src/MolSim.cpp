/**
 * @file MolSim.cpp
 *
 */
#include <iostream>
#include <list>

#include "FileReader.h"
#include "ParticleContainer.h"
#include "PlanetSimulation.h"
#include "Simulation.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

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

constexpr double start_time = 0;
double end_time = 1000;
double delta_t = 0.014;

ParticleContainer particleContainer;

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;

  // should only work if filename is passed, or optional endtime AND delta_t is passed
  if (!(argc == 2 || argc == 4)) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "./Molsym <filename>" << std::endl;
    std::cout << "./Molsym <filename> <end_time> <delta_t>" << std::endl;
    std::cout << "default: endtime = " << end_time << "; delta_t = " << delta_t << std::endl;
    return -1;
  }

  // parse endtime and delta_t
  if (argc == 4) {
    try {
      end_time = std::stod(argsv[2]);
      delta_t = std::stod(argsv[3]);
    } catch (std::invalid_argument &e) {
      std::cout << "Error: could not parse arguments!" << std::endl;
      std::cout << "expected: double" << std::endl;
      return -1;
    }
  }

  // use given parameters, or default endtime = 1000 delta_t = 0.014
  std::cout << "Starting simulation with parameters:" << std::endl
            << "endtime = " << end_time << std::endl
            << "delta_t = " << delta_t << std::endl;

  FileReader fileReader;
  fileReader.readFile(particleContainer.particles, argsv[1]);

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

void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particleContainer.particles, out_name, iteration);
}
