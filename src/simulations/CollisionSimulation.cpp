//
// Created by jannik on 11/23/25.
//
#include "CollisionSimulation.h"

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "../utils/ArrayUtils.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "Physics.h"
#include "particleContainers/LinkedCellsContainer.h"

CollisionSimulation::CollisionSimulation(ParticleContainerV2 &container, Vector3 dimension, double end_time,
                                         double delta_t, double cutoffRadius, std::array<BorderType, 6> &border,
                                         bool is2D)
    : end_time(end_time),
      delta_t(delta_t),
      cutoffRadius(cutoffRadius),
      container(container),
      repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma),
      is2D(is2D) {
  initializeBrownianMotion();
}

void CollisionSimulation::iteration() {
  spdlog::debug("Updating Positions");
  updateX();
  spdlog::debug("Updating Forces");
  updateF();
  spdlog::debug("Updating Velocities");
  updateV();
}

void CollisionSimulation::updateF() {
  auto lennardJonesHelper = [this](Particle &p1, Particle &p2) {
    Vector3 f = Physics::lennardJonesForce(p1, p2, this->sigma, this->epsilon);
    return f;
  };
  container.applyToAllPairs(lennardJonesHelper);
}

void CollisionSimulation::updateX() { container.updatePosition(Physics::calculateX); }

void CollisionSimulation::updateV() { container.updateVelocity(Physics::calculateV); }

void CollisionSimulation::initializeBrownianMotion() {
  auto applyBrownianMotion = [this](Particle &p) {
    Vector3 v = p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, (is2D ? 2 : 3));
    return v;
  };
  container.applyToAllParticles(applyBrownianMotion, 1);
}
