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

  virtual void applyToAllPairs(std::function<Vector3(Particle &p1, Particle &p2)> forceCalculation) = 0;
  /**
   *
   * @param apply function returning the vector being applied
   * @param k integer {0, 1, 2], that determines, what should be applied (position = 0, velocity = 1, force = 2)
   */
  virtual void applyToAllParticles(std::function<Vector3(Particle &)> apply, int k) = 0;
  virtual void updatePosition(Vector3 calculateX(Particle &p, double delta_t)) = 0;
  virtual void updateVelocity(Vector3 calculateV(Particle &p, double delta_t)) = 0;
};
