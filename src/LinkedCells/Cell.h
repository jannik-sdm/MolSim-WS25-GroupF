//
// Created by jv_fedora on 19.11.25.
//
#pragma once
#include "../ParticleContainer.h"
#include "../Particle.h"
/**
 * Cell Type
 * REGULAR = Cell
 */
enum CellType {
  REGULAR,
  BORDER,
  GHOST
};

enum BorderType {
  OUTFLOW,
  REFLECTION,
  PERIODIC
};


class Cell {
public:
    /**
   * Vector that contains pointers to all particles currently in the cell
   */
  std::vector<Particle*> particles;

  /**
  * Vector that contains Ghost Particles (real Particles and not only references to particles)
  */
  std::vector<Particle> ghostParticles;

   /**
    * Describes, if it is an inner cell (regular), an edge cell or a ghost cell
    */
  CellType cell_type;
  /**
  * Describes what kind of Boundary this cell has. on each side. Regular inner cells will have boundary type OUTFLOW in each direction:
  * borders[0] -> (0,0,0) (0,1,1)
  * borders[1] -> (0,0,0) (1,0,1)
  * borders[2] -> (0,0,0) (1,1,0)
  * borders[3] -> (1,0,0) (1,1,1)
  * borders[4] -> (0,1,0) (1,1,1)
  * borders[5] -> (0,0,1) (1,1,1)
  */
  std::array<BorderType, 6> borders;


  Cell (ParticleContainer particleContainer, CellType type);

  // default constructur:
  Cell() : cell_type(REGULAR) {borders.fill(OUTFLOW);}

  CellType getCellType();
  std::vector<Particle*> getParticles();
  bool inContainer(Particle particle);


};
