#pragma once

#include <algorithm>
#include <vector>

#include "Particle.h"
/**
 * @file ParticleContainer.h
 */

/**
 * @class PairIterator
 * @brief Iterator over distinct pairs
 *
 * Provides an iterator to iterate over all distinct pairs in the container, equivalent to
 * ```
 * for (int i = 0; i < particleContainer.size(); i++){
 *     for (int j = i + 1; j < particleContainer.size(); j++) {
 *         // do something
 *     }
 * }
 * ```
 */
class PairIterator {
  std::vector<Particle> &particles;
  /**
   * @brief Index of the first member in the container
   */
  size_t i;
  /**
   * @brief Index of the second member in the container
   */
  size_t j;

 public:
  PairIterator(std::vector<Particle> &particles, size_t i, size_t j) : particles(particles), i(i), j(j){};
  std::pair<Particle &, Particle &> operator*();
  /**
   * Moves the iterator to the next distinct pair of particles
   */
  PairIterator &operator++();
  /**
   * @brief Check for equality between two PairIterators
   *
   * @param other Other PairIterator to compare to
   * @return true if they are at the same pair,
   * @return false if not
   */
  bool operator==(const PairIterator &other) const;
  /**
   * @brief Check for inequality between two PairIterators
   *
   * @param other Other PairIterator to compare to
   * @return true if they are not at the same pair,
   * @return false if they are
   */
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
  PairRange(std::vector<Particle> &particles) : particles(particles){};

  /**
   * @brief Begin iterating through distinct pairs
   *
   * @return PairIterator at the start of the list of distinct pairs
   */
  PairIterator begin() { return PairIterator(particles, 0, 1); }

  /**
   * @brief Stop iterating through distinct pairs
   *
   * @return PairIterator at the end of the list of distinct pairs
   */
  PairIterator end() {
    return PairIterator(particles, std::max(0UL, particles.size() - 2), std::max(1UL, particles.size() - 1));
  }
};

/**
 * @class ParticleContainer
 * @brief Container for storing particles
 *
 * This class stores particles in a container to allow for different iteration methods over the underlying data
 * structure. At the moment, linear iteration is handled by proxying the default iterator of std::vector
 */
class ParticleContainer {
  /**
   * Type alias for the standard vector iterator
   */
  using iterator = std::vector<Particle>::iterator;
  /**
   * Type alias for the standard const vector iterator
   */
  using const_iterator = std::vector<Particle>::const_iterator;

 public:
  /**
   * Data structure for storing the particles
   */
  std::vector<Particle> &particles;

  ParticleContainer(std::vector<Particle> &p) : particles(p) {}

  /**
   * @brief Proxy for `std::vector begin()`
   */
  iterator begin() { return particles.begin(); }
  /**
   * @brief Proxy for `std::vector end()`
   */
  iterator end() { return particles.end(); }
  /**
   * @brief Const proxy for `std::vector begin()`
   */
  const_iterator begin() const { return particles.begin(); }
  /**
   * @brief Const proxy for `std::vector end()`
   */
  const_iterator end() const { return particles.end(); }
  /**
   * @brief Proxy for `std::vector cbegin()`
   */
  const_iterator cbegin() const { return particles.cbegin(); }
  /**
   * @brief Proxy for `std::vector cend()`
   */
  const_iterator cend() const { return particles.cend(); }

  PairRange pairs() { return PairRange(particles); }
};
