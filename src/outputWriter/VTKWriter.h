/*
 * VTKWriter.h
 *
 *  Created on: 01.03.2010
 *      Author: eckhardw
 */

#pragma once
#ifdef ENABLE_VTK_OUTPUT

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <list>
#include <string>

#include "Particle.h"

namespace outputWriter {

/**
 * This class implements the functionality to generate vtk output from
 * particles using the official VTK library.
 */
class VTKWriter {
 public:
  VTKWriter() = default;
  ~VTKWriter() = default;

  // Delete copy constructor and assignment operator
  VTKWriter(const VTKWriter &) = delete;
  VTKWriter &operator=(const VTKWriter &) = delete;

  /**
   * Add a particle's data to the output.
   * @note initializeOutput() must be called before this method.
   * @param p Particle to add to the output
   */
  void plotParticles(std::list<Particle> particles, const std::string &filename, int iteration);
};

}  // namespace outputWriter
#endif