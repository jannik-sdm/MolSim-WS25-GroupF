//
// Created by jannik on 11/6/25.
//

#include "PlanetSimulation.h"

#include "../container.h"
#include "../utils/ArrayUtils.h"
#include "Physics.h"
#include "Simulation.h"

PlanetSimulation::PlanetSimulation(containerV2 &container, const double end_time, const double delta_t)
    : container(container), end_time(end_time), delta_t(delta_t) {}

void PlanetSimulation::iteration() {
  // calculate new x
  updateX();
  // calculate new f
  updateF();
  // calculate new v
  updateV();
}

void PlanetSimulation::updateF() {
  for (auto &p : container) p.setF({0, 0, 0});
  for (auto [p1, p2] : container.pairs()) {
    Vector3 f = Physics::planetForce(p1, p2);

    p1.addF(f);
    p2.subF(f);
  }
  container.applyToAllPairs(Physics::planetForce);
}

void PlanetSimulation::updateX() {
  for (auto &p : container) {
    p.setX(Physics::calculateX(p, delta_t));
  }
}

void PlanetSimulation::updateV() {
  for (auto &p : container) {
    p.setV(Physics::calculateV(p, delta_t));
  }
}
