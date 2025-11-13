#pragma once

#include <fstream>
#include <vector>

#include "FileReader.h"

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
  static void parse(std::vector<Particle> &particles, std::istream &file);
};
