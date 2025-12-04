//
// Created by jannik on 12/3/25.
//
#pragma once
#include <vector>

#include "ParticleContainerV2.h"
class DirectSum : public ParticleContainerV2 {
  double delta_t;

 public:
  DirectSum(std::vector<Particle> &particles, double delta_t, double sigma, double epsilon)
      : ParticleContainerV2(particles), delta_t(delta_t) {};
  void applyToAllPairs(std::function<Vector3(Particle &p1, Particle &p2)> calculateForce) override;
  void applyToAllParticles(std::function<Vector3(Particle &)> apply, int k) override;
  void updatePosition(Vector3 calculateX(Particle &p, double delta_t)) override;
  void updateVelocity(Vector3 calculateV(Particle &p, double delta_t)) override;
};
