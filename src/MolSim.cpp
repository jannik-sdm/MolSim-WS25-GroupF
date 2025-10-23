
#include <iostream>
#include <list>

#include "FileReader.h"
#include "outputWriter/XYZWriter.h"
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

/**
 * Calculates the euclidian norm for a vector
 * \f$∥v∥₂ = \sqrt{x² + y² + z²}\f$
 */
double norm(const std::array<double, 3> &v);

constexpr double start_time = 0;
constexpr double end_time = 1000;
constexpr double delta_t = 0.014;

// TODO: what data structure to pick?
std::list<Particle> particles;

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  FileReader fileReader;
  fileReader.readFile(particles, argsv[1]);

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
  std::list<Particle>::iterator iterator;
  iterator = particles.begin();

  for (auto &p1 : particles) {
    std::array<double, 3> Σ = {0,0,0};

    for (auto &p2 : particles) {
      // @TODO: insert calculation of force
      if (p1 == p2) continue;

      const double a = 1 / pow(norm(p1.getX() - p2.getX()),3);
      auto f = a * p1.getM() * p2.getM() * (p2.getX() - p1.getX());

      Σ = Σ + f;
    }

    p1.setF(Σ);
  }
}
//
double norm(const std::array<double, 3> &v) {
  return sqrt(pow(v[0], 2)+pow(v[1], 2)+pow(v[2], 2));
}

void calculateX() {
  for (auto &p : particles) {
    // @TODO: insert calculation of position updates here!
    const double a = 1 / (2 * p.getM());
    p.setX(p.getX() + delta_t * p.getV() + pow(delta_t, 2) * a * p.getF());
  }
}

void calculateV() {
  for (auto &p : particles) {
    // @TODO: insert calculation of velocity updates here!
    const double a = 1 / (2 * p.getM());
    p.setV(p.getV() + delta_t * a * (p.getOldF() + p.getF()));
  }
}

void plotParticles(int iteration) {
  std::string out_name("MD_vtk");


  outputWriter::XYZWriter writer;
  writer.plotParticles(particles, out_name, iteration);
}
