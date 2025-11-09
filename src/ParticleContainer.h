#pragma once

#include <algorithm>
#include <vector>

#include "Particle.h"
/**
 * @file ParticleContainer.h
 */

class PairIterator {
  std::vector<Particle> &particles;
  size_t i;
  size_t j;

 public:
  PairIterator(std::vector<Particle> &particles, size_t i, size_t j) : particles(particles), i(i), j(j) {};
  std::pair<Particle &, Particle &> operator*();
  PairIterator &operator++();
  bool operator==(const PairIterator &other) const;
  bool operator!=(const PairIterator &other) const;
};

/**
 * @class PairRange
 * @brief Provides a Range to iterate over *all* pairs in a vector
 *
 * This class enables the usage of the syntax
 * ```cpp
 * for (auto [p1, p2] : ParticleContainer.pairs())
 * ```
 * instead of having to use the iterator manually
 */
class PairRange {
 private:
  std::vector<Particle> &particles;

 public:
  PairRange(std::vector<Particle> &particles) : particles(particles) {};

  PairIterator begin() { return PairIterator(particles, 0, 1); }
  PairIterator end() {
    return PairIterator(particles, std::max(0UL, particles.size() - 2), std::max(1UL, particles.size() - 1));
  }
};

class ParticleContainer {
  using iterator = std::vector<Particle>::iterator;
  using const_iterator = std::vector<Particle>::const_iterator;

 public:
  std::vector<Particle> particles;
  iterator begin() { return particles.begin(); }
  iterator end() { return particles.end(); }
  const_iterator begin() const { return particles.begin(); }
  const_iterator end() const { return particles.end(); }
  const_iterator cbegin() const { return particles.cbegin(); }
  const_iterator cend() const { return particles.cend(); }

  PairRange pairs() { return PairRange(particles); }
};