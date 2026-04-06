#pragma once

#include <fstream>
#include <vector>

#include "inputReader/FileReader.h"

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
  /**
   * @brief Parses a stream of the `file` in cuboid format into `particles`
   *
   * @param particles Where to store the read particles
   * @param file Filestream to read from
   */
  static void parse(std::vector<Particle> &particles, std::istream &file);
};
