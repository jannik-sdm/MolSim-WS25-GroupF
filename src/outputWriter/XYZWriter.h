/**
 * @file XYZWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once

#include <fstream>
#include <vector>

#include "Particle.h"

namespace outputWriter {

/**
 * @class XYZWriter
 *
 * This class implements functionality to generate xyz output from a particle vector
 */
class XYZWriter {
 public:
  XYZWriter();

  virtual ~XYZWriter();

  /**
   * Write XYZ output of particles.
   * @param particles Particles to add to the output
   * @param filename Output filename
   * @param iteration Current iteration number
   */
  void plotParticles(std::vector<Particle> &particles, const std::string &filename, int iteration);
};

}  // namespace outputWriter
