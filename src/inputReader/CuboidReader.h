#pragma once

#include <fstream>
#include <vector>

#include "FileReader.h"

/**
 * @class CuboidReader
 * @brief Parse particles from cuboid instructions
 *
 * CRTP implementation of Cuboid files.
 * Each line contains instructions for building a cuboid of particles.
 * An example would be:
 * ```
 * # xyz-coord     n.o.particles   distance    mass    velocity
 * 0.0 0.0 0.0     0.0 0.0 0.0     0.0         0.0     0.0 0.0 0.0
 * ```
 *
 * @see FileReader
 */
class CuboidReader : public FileReader<CuboidReader> {
 public:
  static void parse(std::vector<Particle> &particles, std::ifstream &file);
};
