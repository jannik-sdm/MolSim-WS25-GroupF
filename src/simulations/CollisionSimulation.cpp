//
// Created by jannik on 11/7/25.
//

#include "CollisionSimulation.h"
#include "../utils/ArrayUtils.h"
#include "../utils/MaxwellBoltzmannDistribution.h"

CollisionSimulation::CollisionSimulation(ParticleContainer &container, const double end_time, const double delta_t) : PlanetSimulation(container, end_time, delta_t) {
  // initialize particles with brownian motion
  for (auto &p : particleContainer) {
    Vector3 v = maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, 2);
    p.setV(p.getV() + v);
  }

}
void CollisionSimulation::calculateF() {
  for (auto &p : particleContainer) p.setF({0, 0, 0});
  for (auto [p1, p2] : particleContainer.pairs()) {
    // force calculation
    const double norm = ArrayUtils::L2Norm(p1.getX() - p2.getX());
    const double a = pow(sigma / norm, 6) - 2 * pow(sigma / norm, 12);
    const double b = -(24*epsilon) / pow(norm, 2);
    const Vector3 force = a*b*(p1.getX()- p2.getX());

    p1.addF(force);
    p2.subF(force);
  }
}
