#pragma once

#include <vector>

#include "Particle.h"
/**
 * @file ParticleContainer.h
 */

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

  class PairIterator {
    std::vector<Particle> &particles;
    size_t i;
    size_t j;

   public:
    PairIterator(std::vector<Particle> &particles, size_t i, size_t j) : particles(particles), i(i), j(j){};
    std::pair<Particle &, Particle &> operator*();
    /**
     * This iterator only iterates over pairwise distinct values ​​and returns each combination of particles only
     * once.
     */
    PairIterator &operator++();
    bool operator==(const PairIterator &other) const;
    bool operator!=(const PairIterator &other) const;
  };

  PairIterator pairs_begin() { return PairIterator(particles, 0, 1); }
  PairIterator pairs_end() { return PairIterator(particles, particles.size() - 1, particles.size() - 1); }
};
