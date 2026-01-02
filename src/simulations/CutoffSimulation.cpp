//
// Created by jannik on 11/23/25.
//
#include "simulations/CutoffSimulation.h"

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "Physics.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void CutoffSimulation::iteration() {
  spdlog::debug("Updating Positions");
  updateX();
  spdlog::debug("Updating Forces");
  updateF();
  spdlog::debug("Updating Velocities");
  updateV();
}
void CutoffSimulation::updateF() {
  // set the force of all particles to zero
  linkedCells.applyToParticles([](Particle &p) { p.setF({0, 0, 0}); });

  linkedCells.applyToPairs([this](Particle &p1, Particle &p2) {
    Vector3 f = Physics::LennardJones::force(p1, p2, sigma, epsilon);
    p1.addF(f);
    p2.subF(f);
  });
}

void CutoffSimulation::updateX() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getState() < 0) return;
    spdlog::trace("Updating X:");
    spdlog::trace("-> Old Position: ({},{},{})", p.getX()[0], p.getX()[1], p.getX()[2]);
    p.setX(Physics::StoermerVerlet::position(p, delta_t));
    spdlog::trace("-> New: ({},{},{})", p.getX()[0], p.getX()[1], p.getX()[2]);
  });
  linkedCells.moveParticles();
}

void CutoffSimulation::updateV() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getState() < 0) return;
    spdlog::trace("Updating V:");
    spdlog::trace("-> Old Velocity: ({},{},{})", p.getV()[0], p.getV()[1], p.getV()[2]);
    p.setV(Physics::StoermerVerlet::velocity(p, delta_t));
    spdlog::trace("-> New Velocity: ({},{},{})", p.getV()[0], p.getV()[1], p.getV()[2]);
  });
}

void CutoffSimulation::initializeBrownianMotion() {
  linkedCells.applyToParticles([this](Particle &p) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, (is2D ? 2 : 3)));
  });
}