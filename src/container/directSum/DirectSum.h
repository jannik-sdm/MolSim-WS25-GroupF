#pragma once

#include <array>
#include <vector>

#include "Particle.h"

class DirectSum {
 private:
  std::vector<Particle> &particles;

 public:
  explicit DirectSum(std::vector<Particle> &particles) : particles(particles) {}

  template <typename Function, typename... Args>
  inline void applyToParticles(Function f, Args &&...args) {
    for (auto &p : particles) f(p, std::forward<Args>(args)...);
  }

  template <typename Function>
  inline void applyToPairs(Function f) {
    for (auto i = particles.begin(); i != particles.end(); i++)
      for (auto j = std::next(i); j != particles.end(); j++) f(*i, *j);
  }
};