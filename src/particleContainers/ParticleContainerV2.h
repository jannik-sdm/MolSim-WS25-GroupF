//
// Created by jannik on 12/3/25.
//

#pragma once
#include <functional>
#include <vector>

#include "../Particle.h"

/**
 * @brief Base class for particle containers
 *
 * This class ensures that every particle container has these methods
 */
class ParticleContainerV2 {
 protected:
  std::vector<Particle> &particles;

 public:
  // desctructor to avoid memory leaks
  virtual ~ParticleContainerV2() = default;
  ParticleContainerV2(std::vector<Particle> &particles) : particles(particles) {}

  virtual void applyToAllPairs(void applyForce(Particle &p1, Particle &p2, double sigma, double epsilon)) = 0;
  virtual void applyToAllParticles(std::function<void(Particle &)> apply) = 0;
  virtual void updatePosition(void updateX(Particle &p, double delta_t)) = 0;
  virtual void updateVelocity(void updateV(Particle &p, double delta_t)) = 0;
};
