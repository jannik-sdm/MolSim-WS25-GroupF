#pragma once

#include <spdlog/spdlog.h>

#include <unordered_map>

#include "Particle.h"
#include "container/directSum/ParticleContainer.h"

/**
 * @brief Cell Type
 * Default cells are Regular
 */
enum class CellType { REGULAR, BORDER, GHOST };

/**
 * @brief Border type
 */
enum class BorderType { OUTFLOW, REFLECTION, PERIODIC, NAIVE_REFLECTION };

/**
 * @brief Alias for BorderTypes
 *
 */
using NeighBourIndices = std::array<int, 26>;

/**
 * @class Cell
 * @brief Represents one cell in a linked cells container
 */
class Cell {
 public:
  /**
   * Vector that contains pointers to all particles currently in the cell
   */
  std::vector<Particle *> particles;

  /**
   * Vector that contains Ghost Particles (real Particles and not only references to particles)
   */
  std::vector<Particle> ghost_particles;

  /**
   * Amount of ghost particles currently in the cell
   */
  int size_ghost_particles = 0;  // by keeping track of this we don't have to delete all particles in the vector and can
                                 // instead just overwrite them

  /**
   * Describes, if it is an inner cell (regular), an edge cell or a ghost cell
   */
  CellType cell_type;

  /**
   * Describes what kind of Boundary this cell has on each side. Regular inner cells will have boundary type OUTFLOW in
   * each direction: borders[0] -> (0,0,0) (0,1,1) -> links borders[1] -> (0,0,0) (1,0,1) -> unten borders[2] -> (0,0,0)
   * (1,1,0) -> vorne borders[3] -> (1,0,0) (1,1,1) -> rechts borders[4] -> (0,1,0) (1,1,1) -> oben borders[5] ->
   * (0,0,1) (1,1,1) -> hinten
   */
  std::array<BorderType, 6> borders;

  /**
   * @brief Neighbouring cells
   *
   * Contains indices to all neighbouring cells.
   */
  NeighBourIndices neighbors;

  // default constructur:
  Cell() : cell_type(CellType::REGULAR) { borders.fill(BorderType::OUTFLOW); }

  CellType getCellType();
  std::vector<Particle *> getParticles();
  bool inContainer(Particle particle);
};

inline BorderType string_to_border_type(std::string str) {
  const std::unordered_map<std::string, BorderType> lookup = {
      {"outflow", BorderType::OUTFLOW},
      {"reflection", BorderType::REFLECTION},
      {"periodic", BorderType::PERIODIC},
      {"naive_reflection", BorderType::NAIVE_REFLECTION},
  };

  auto x = lookup.find(str);
  if (x == lookup.end()) {
    spdlog::warn("Invalid border type \"{}\"", str);
    return BorderType::OUTFLOW;
  }

  return x->second;
}
