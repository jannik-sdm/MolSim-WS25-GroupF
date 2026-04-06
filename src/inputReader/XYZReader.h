#pragma once

#include <fstream>
#include <vector>

#include "inputReader/FileReader.h"

/**
 * @class XYZReader
 * @brief Parse particles in the xyz format
 *
 * CRTP implementation of XYZ files.
 * Reads following file format into particles (all doubles):
 * ```
 * # xyz-coord      velocity        mass
 * 0.0 0.0 0.0     0.0 0.0 0.0    0.0
 * ```
 *
 * @see FileReader
 */
class XYZReader : public FileReader<XYZReader> {
 public:
  /**
   * @brief Parses a stream of the `file` in xyz format into `particles`
   *
   * @param particles Where to store the read particles
   * @param file Filestream to read from
   */
  static void parse(std::vector<Particle> &particles, std::istream &file);
};
