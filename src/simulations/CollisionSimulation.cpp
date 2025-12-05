//
// Created by jannik on 11/7/25.
//

#include "CollisionSimulation.h"

#include "../utils/ArrayUtils.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "Physics.h"

CollisionSimulation::CollisionSimulation(std::vector<Particle> &particles, const double end_time, const double delta_t)
    : PlanetSimulation(particles, end_time, delta_t) {
  // initialize particles with brownian motion
  for (auto &p : particleContainer) {
    Vector3 v = maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, 2);
    p.setV(p.getV() + v);
  }
}

void CollisionSimulation::updateF() {
  for (auto &p : particleContainer) p.setF({0, 0, 0});
  for (auto [p1, p2] : particleContainer.pairs()) {
    Vector3 f = Physics::lennardJonesForce(p1, p2, sigma, epsilon);

    p1.addF(f);
    p2.subF(f);
  }
}
