//
// Created by jannik on 11/6/25.
//

#include "PlanetSimulation.h"
#include "Simulation.h"
#include "ParticleContainer.h"
#include "utils/ArrayUtils.h"

PlanetSimulation::PlanetSimulation(const ParticleContainer &container, const double end_time, const double delta_t) :
  particleContainer(container), end_time(end_time), delta_t(delta_t) {
}
void PlanetSimulation::iteration() {
  // calculate new x
  calculateX();
  // calculate new f
  calculateF();
  // calculate new v
  calculateV();
}

void PlanetSimulation::calculateF() {
  for (auto &p : particleContainer) p.setF({0, 0, 0});
  for (auto it = particleContainer.pairs_begin(); it != particleContainer.pairs_end(); ++it) {
    auto [p1, p2] = *it;

    const double a = 1 / pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 3);
    auto f = a * p1.getM() * p2.getM() * (p2.getX() - p1.getX());

    p1.addF(f);
    p2.subF(f);
  }
}

void PlanetSimulation::calculateX() {
  for (auto &p : particleContainer) {
    // @TODO: insert calculation of position updates here!
    const double a = 1 / (2 * p.getM());
    p.setX(p.getX() + delta_t * p.getV() + pow(delta_t, 2) * a * p.getF());
  }
}

void PlanetSimulation::calculateV() {
  for (auto &p : particleContainer) {
    // @TODO: insert calculation of velocity updates here!
    const double a = 1 / (2 * p.getM());
    p.setV(p.getV() + delta_t * a * (p.getOldF() + p.getF()));
  }
}
