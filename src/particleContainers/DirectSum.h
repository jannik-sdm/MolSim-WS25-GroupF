//
// Created by jannik on 12/3/25.
//
#pragma once
#include <vector>

#include "ParticleContainerV2.h"
class DirectSum : public ParticleContainerV2 {
  double sigma;
  double epsilon;
  double delta_t;

 public:
  DirectSum(std::vector<Particle> &particles, double delta_t, double sigma, double epsilon)
      : ParticleContainerV2(particles), delta_t(delta_t), sigma(sigma), epsilon(epsilon) {};
  void applyToAllPairs(void applyForce(Particle &p1, Particle &p2, double sigma, double epsilon)) override;
  void applyToAllParticles(std::function<void(Particle &)> apply) override;
  void updatePosition(void updateX(Particle &p, double delta_t)) override;
  void updateVelocity(void updateV(Particle &p, double delta_t)) override;
};
