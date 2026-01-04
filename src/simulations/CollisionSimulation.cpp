//
// Created by jannik on 11/7/25.
//

#include "simulations/CollisionSimulation.h"

#include "Physics.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void CollisionSimulation::updateF() {
  container.applyToParticles([](Particle &p) { p.setF({0, 0, 0}); });
  container.applyToPairs([this](Particle &p1, Particle &p2) {
    const Vector3 f = Physics::LennardJones::force(p1, p2, sigma, epsilon);
    p1.addF(f);
    p2.subF(f);
  });
}

void CollisionSimulation::initializeBrownianMotion(const double brown_motion_avg_velocity) {
  container.applyToParticles([brown_motion_avg_velocity](Particle &p) {
    const Vector3 v = maxwellBoltzmannDistributedVelocity(brown_motion_avg_velocity, 2);
    p.setV(p.getV() + v);
  });
}
