//
// Created by jannik on 11/6/25.
//

#include "PlanetSimulation.h"

#include "../utils/ArrayUtils.h"
#include "Physics.h"
#include "Simulation.h"

PlanetSimulation::PlanetSimulation(ParticleContainerV2 &container, const double end_time, const double delta_t)
    : container(container), end_time(end_time), delta_t(delta_t) {}

void PlanetSimulation::iteration() {
  // calculate new x
  updateX();
  // calculate new f
  updateF();
  // calculate new v
  updateV();
}

void PlanetSimulation::updateF() { container.applyToAllPairs(Physics::planetForce); }

void PlanetSimulation::updateX() { container.updatePosition(Physics::calculateX); }

void PlanetSimulation::updateV() { container.updateVelocity(Physics::calculateV); }
