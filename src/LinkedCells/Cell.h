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
enum cell_type {
  REGULAR,
  EDGE,
  GHOST
};

class Cell {
  private:
  /**
   * Data structure which contains all particles of this cell
   */
  ParticleContainer particleContainer;

   /**
    * Describes, if it is an inner cell (regular), an edge cell or a ghost cell
    */
  cell_type particleType;


public:

  Cell (ParticleContainer particleContainer, cell_type particleType);

  // default constructur:
  Cell() : particleType(REGULAR) {}

  cell_type getParticleType();
  ParticleContainer getParticleContainer ();
  bool inContainer(Particle particle);


};
