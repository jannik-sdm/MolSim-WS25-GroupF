//
// Created by jannik on 11/6/25.
//

#include "simulations/PlanetSimulation.h"

#include "Physics.h"
#include "container/directSum/ParticleContainer.h"
#include "simulations/Simulation.h"
#include "utils/ArrayUtils.h"

PlanetSimulation::PlanetSimulation(std::vector<Particle> &particles, const double end_time, const double delta_t)
    : particleContainer(particles), end_time(end_time), delta_t(delta_t) {}

void PlanetSimulation::iteration() {
  // calculate new x
  updateX();
  // calculate new f
  updateF();
  // calculate new v
  updateV();
}

void PlanetSimulation::updateF() {
  for (auto &p : particleContainer) p.setF({0, 0, 0});
  for (auto [p1, p2] : particleContainer.pairs()) {
    Vector3 f = Physics::planetForce(p1, p2);

    p1.addF(f);
    p2.subF(f);
  }
}

void PlanetSimulation::updateX() {
  for (auto &p : particleContainer) {
    p.setX(Physics::calculateX(p, delta_t));
  }
}

void PlanetSimulation::updateV() {
  for (auto &p : particleContainer) {
    p.setV(Physics::calculateV(p, delta_t));
  }
}
