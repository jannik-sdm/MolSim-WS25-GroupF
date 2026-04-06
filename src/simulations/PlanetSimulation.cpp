//
// Created by jannik on 11/6/25.
//

#include "simulations/PlanetSimulation.h"

#include "Physics.h"
#include "container/directSum/ParticleContainer.h"
#include "simulations/Simulation.h"
#include "utils/ArrayUtils.h"

void PlanetSimulation::iteration() {
  // calculate new x
  updateX();
  // calculate new f
  updateF();
  // calculate new v
  updateV();
}

void PlanetSimulation::updateF() {
  container.applyToParticles([](Particle &p) { p.setF({0, 0, 0}); });
  container.applyToPairs([](Particle &p1, Particle &p2) {
    const Vector3 f = Physics::Planet::force(p1, p2);
    p1.addF(f);
    p2.subF(f);
  });
}

void PlanetSimulation::updateX() {
  container.applyToParticles([this](Particle &p) { p.setX(Physics::StoermerVerlet::position(p, delta_t)); });
}

void PlanetSimulation::updateV() {
  container.applyToParticles([this](Particle &p) { p.setV(Physics::StoermerVerlet::velocity(p, delta_t)); });
}
