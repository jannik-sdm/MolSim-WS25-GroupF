#pragma once

#include <omp.h>

#include <array>
#include <vector>

#include "Particle.h"

/**
 * @class DirectSum
 * DirectSum container used in Assignment 1 and 2.
 *
 * Allows for iteration over single particles or distinct pairs
 */
class DirectSum {
 private:
  /**
   * Reference to the particles vector
   */
  std::vector<Particle> &particles;

 public:
  explicit DirectSum(std::vector<Particle> &particles) : particles(particles) {}

  /**
   * @brief Applies the given function to all particles in the simulation
   */
  template <typename Function, typename... Args>
  inline void applyToParticles(Function f, Args &&...args) {
#pragma omp parallel for
    for (auto &p : particles) f(p, std::forward<Args>(args)...);
  }

  template <typename Function, typename... Args>
  inline void applyToParticlesPlotting(Function f, Args &&...args) {
    for (auto &p : particles) f(p, std::forward<Args>(args)...);
  }

  /**
   * @brief Applies the given function to all particle pairs in the simulation
   */
  template <typename Function>
  inline void applyToPairs(Function f) {
#pragma omp parallel for collapse(2)
    for (auto i = 0; i < particles.size(); i++) {
      for (auto j = i + 1; j < particles.size(); j++) {
        f(particles[i], particles[j]);
      }
    }
  }

  int getAmoutOfParticles() { return particles.size(); }
};
