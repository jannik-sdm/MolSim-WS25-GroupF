//
// Created by jv_fedora on 19.11.25.
//
#pragma once
#include "../Particle.h"
#include "../ParticleContainer.h"
/**
 * Cell Type
 * REGULAR = Cell
 */
enum CellType { REGULAR, BORDER, GHOST };

// TODO: add enum for type of the cell to handle what happens when particle leaves the cell (outflow, reflectie,
// periodic)

class Cell {
 public:
  /**
   * Vector that contains pointers to all particles currently in the cell
   */
  std::vector<Particle *> particles;

  /**
   * Describes, if it is an inner cell (regular), an edge cell or a ghost cell
   */
  CellType cell_type;

  Cell(ParticleContainer particleContainer, CellType type);

  // default constructur:
  Cell() : cell_type(REGULAR) {}

  CellType getCellType();
  std::vector<Particle *> getParticles();
  bool inContainer(Particle particle);
};
