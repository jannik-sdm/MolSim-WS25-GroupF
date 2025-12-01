#pragma once

#include <array>
#include <vector>

#include "Particle.h"

/**
 * @class ParticleGenerator
 * @brief Utility class for generating Particles
 *
 * This class provides static methods for generating particles in various arrangements
 * The first argument of any generator should always be a reference to the particle vector.
 * New particles should always be appended, not replaced.
 */
class ParticleGenerator {
 public:
  /**
   * @brief Generate a cuboid of particles
   * Creates a cuboid of particles with a given distance
   *
   * @param[in, out] particles Vector to append the generated particles
   * @param[in] x Left corner of the cuboid
   * @param[in] n Number of particles along each axis
   * @param[in] distance Distance between particles
   * @param[in] mass Default Mass for each particle
   * @param[in] v Default Velocity for each particle
   */
  static void cuboid(std::vector<Particle> &particles, const Vector3 x, const std::array<unsigned int, 3> n,
                     const double distance, const double mass, Vector3 v);

  /**
   * @brief Generate a disc of particles
   *
   * Appends a disk of particles into `particles`.
   * The disk is centered at `position` and consists of a grid of distance `distance` and `radius` particles in each
   * direction.
   *
   * @param particles Where to place new particles
   * @param position Center of the disk
   * @param r Radius of the disc as number of particles in each direction
   * @param distance distance between particles
   * @param mass mass of each particle
   * @param velocity initial velocity of each particle
   */
  static void disc(std::vector<Particle> &particles, const Vector3 position, const int radius, const double distance,
                   const double mass, const Vector3 velocity);
};
