//
// Created by jannik on 11/6/25.
//

#include "PlanetSimulation.h"

#include "../ParticleContainer.h"
#include "../utils/ArrayUtils.h"
#include "Physics.h"
#include "Simulation.h"

PlanetSimulation::PlanetSimulation(ParticleContainer &container, const double end_time, const double delta_t)
    : particleContainer(container), end_time(end_time), delta_t(delta_t) {}
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
  for (auto [p1, p2] : particleContainer.pairs()) {
    Vector3 f = Physics::planetForce(p1, p2);

    p1.addF(f);
    p2.subF(f);
  }
}

void PlanetSimulation::calculateX() {
  for (auto &p : particleContainer) {
    p.setX(Physics::calculateX(p, delta_t));
  }
}

void PlanetSimulation::calculateV() {
  for (auto &p : particleContainer) {
    p.setV(Physics::calculateV(p, delta_t));
  }
}
