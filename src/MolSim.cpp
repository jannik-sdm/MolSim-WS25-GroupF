
#include <iostream>
#include <list>

#include "FileReader.h"
#include "ParticleContainer.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/VTKWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();

/**
 * plot the particles to a xyz-file
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
