//
// Created by jv_fedora on 19.11.25.
//

#pragma once

#include <array>
#include <vector>

#include "container/directSum/ParticleContainer.h"
#include "container/linkedCells/Cell.h"
#include "utils/ArrayUtils.h"

class LinkedCells {
 public:
  /**
   * 1-D Array with all Cells
   */
  std::vector<Cell> cells;

  /**
   * Reference to a Vector of all particles in the simulation
   */
  std::vector<Particle> &particles;

  /**
   * amount of particles still alive
   */
  int alive_particles = 0;

  /**
   * Domain size of the simulation
   */
  std::array<double, 3> domain_size;

  /**
   * Cell size of each cell in the simulation
   */
  std::array<double, 3> cell_size;
  /**
   * Describes how many cells the overall structure has in X-direction
   */
  int numCellsX;
  /**
Describes how many cells the overall structure has in Y-direction
*/
  int numCellsY;
  /**
   * Describes how many cells the overall structure has in Z-direction

   */
  int numCellsZ;

  //  Für die Höhe, Länge und Breite einer einzelnen Zelle. Es kann sein, das wir die Info aber garnicht brauchen
  /**
  Describes the size of each cell  X-direction
  */
  double cellSizeX;
  /**
  Describes the size of each cell  Y-direction
  */
  double cellSizeY;
  /**
  Describes the size of each cell  Z-direction
  */
  double cellSizeZ;

  const double cutoffRadius = 3.0;

  bool is2D;

  double repulsing_distance;
  /**
   * Initializes the variables and cells with their cell-type and adds the respective particles to the cell
   * @param size_x domain size in x direction
   * @param size_y domain size in y direction
   * @param size_z domain size in z direction
   * @param cutoff cutoff radius set in the simluation
   */
  LinkedCells(std::vector<Particle> &particles, const Vector3 domain, const double cutoff, bool is2D,
              double repulsing_distance, std::array<BorderType, 6> borders = {BorderType::OUTFLOW});

  /**
   *
   * @tparam Function
   * @param f A function modifying a pair of particles
   * @brief Iterates over all pairs of particles in the simulation, according to the linked cells algorithm and applies
   * the function f
   */
  template <typename Function>
  inline void applyToPairs(Function f) {
    // set the force of all particles to zero
    for (Particle &particle : particles) particle.setF({0, 0, 0});

    // Calculate forces in own cell
    for (Cell &cell : cells) {
      for (int i = 0; i < cell.particles.size(); i++) {
        const auto p1 = cell.particles[i];

        for (int j = i + 1; j < cell.particles.size(); j++) {
          const auto p2 = cell.particles[j];

          if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

          // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
          f(*p1, *p2);
          // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
          // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
        }
      }
    }

    // Calculate forces with neighbour cells
    for (int i = 0; i < cells.size(); i++) {
      auto &c1 = cells[i];

      // skip ghost cells
      if (c1.cell_type == CellType::GHOST) continue;
      NeighBourIndices neighbourCellsIndex = getNeighbourCells(i);

      for (const int j : neighbourCellsIndex) {
        auto &c2 = cells[j];
        // newton optimization, but ONLY if c2 is not a Ghost cell, because if this calculation is skipped, particles
        // are not repulsed
        if (j < i && c2.cell_type != CellType::GHOST) continue;

        for (const auto p1 : c1.particles) {
          // iterate over ghost particles if c2 is a ghost cell, else use normale particles
          if (c2.cell_type == CellType::GHOST) {
            for (int k = 0; k < c2.size_ghost_particles; k++) {
              Particle &p2 = c2.ghost_particles[k];
              const double distance = ArrayUtils::L2Norm(p1->getX() - p2.getX());
              // for ghost particles the force should only be computed if its repulsing
              // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
              spdlog::trace("reached radius check for ghost particles");
              if (distance >= this->repulsing_distance || distance > cutoffRadius) continue;

              f(*p1, p2);
            }
          } else {
            // case for regular cells
            for (const auto p2 : c2.particles) {
              if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

              f(*p1, *p2);
              // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
            }
          }
        }
      }
    }
  };

  /**
   *
   * @tparam Function
   * @param f A function modifying a particle
   * @brief Iterates over all particles in the simulation and applies the function f
   */
  template <typename Function>
  inline void applyToParticles(Function f) {
    for (auto &p : particles) {
      f(p);
    }
  };

  /**
   * @brief Moves the particles that left a cell into their new cell
   */
  void moveParticles();

 private:
  /**
   * Finds the neighbour-cells of the given cell and returns their cell-array indexes
   * @param cellIndex 1D cell index of the current cell
   * @returns array of 1D cell indexes
   */
  NeighBourIndices getNeighbourCells(int cellIndex) { return cells[cellIndex].neighbors; }

  /**
   * Helper Function for the Constructor. Finds the neighbourcells of the given Cell and writes them
   * in the NeighbourCells Attribut of the cell
   * @param cellIndex cell to find the neighbours
   */
  void setNeighbourCells(int cellIndex);

  /**
   * Calculates the 3D index of a cell from the 1D index in the array
   * @param cellIndex 1D index in the array of cells
   * @return 3D index of the cell in a cube
   */
  std::array<int, 3> index1dToIndex3d(int cellIndex);

  int index3dToIndex1d(int x, int y, int z);

  /**
   * Calculates the 1D CellIndex of a 3D Position
   * @param x x-position of the Cell
   * @param y y-position of the Cell
   * @param z z-position of the Cell
   * @return CellIndex
   */
  int coordinate3dToIndex1d(double x, double y, double z);

  int coordinate3dToIndex1d(const Vector3 &x) { return coordinate3dToIndex1d(x[0], x[1], x[2]); }

  std::array<int, 3> coordinate3dToIndex3d(double x, double y, double z);

  /**
   * Calculates the distance between a given position and a given border of a cell
   * @param cellIndex1d 1D index of the cell
   * @param border border number between 0 and 5 (border 0, 3 -> x-direction, border 1,4 -> y-direction, border 2,5 ->
   * z-direction, 0,1,2 -> min-border, 3,4,5 -> max-border)
   * @param pos position to calculate the distance to
   * @return distance between the position and the border
   */

  double getBorderDistance(int cellIndex1d, int border, Vector3 pos);

  /**
   * Calculates the Border between two neighbour Cells
   * @param ownIndex1d first 1D Inex of the neighbour Cells
   * @param otherIndex1d second 1D Index of the neighbour Cells
   * @return border number between -1 and 5 (border 0, 3 -> x-direction, border 1,4 -> y-direction, border 2,5 ->
   * z-direction, 0,1,2 -> min-border, 3,4,5 -> max-border, border -1 -> error. Cells don`t have a common Border)
   * CAUTION! This Implementation always returns only one Border, even if a Particle meight have escaped not through a
   * border, but through an edge or a corner!
   */
  int getSharedBorder(int ownIndex1d, int otherIndex1d);

  /**
   * @brief Checks if a given cell is a ghost cell
   *
   * A cell is a ghost cell when it is at the edge of the domain,
   * i.e when it has a cell index of 0 or the maximum along the respective axis
   *
   * @param x x coordinate of 3d cell index
   * @param y y coordinate of 3d cell index
   * @param z z coordinate of 3d cell index
   * @return true if the particle is at the edge of the domian
   */
  bool isGhostCell(int x, int y, int z) {
    return (x == 0 || y == 0 || z == 0 || x == numCellsX - 1 || y == numCellsY - 1 || z == numCellsZ - 1);
  }

  /**
   * @brief Checks if a given cell is a border cell
   *
   * A cell is a ghost cell when it is at the edge of the domain,
   * i.e when it has a cell index of 1 or one less than the maximum along the respective axis
   *
   * @param x x coordinate of 3d cell index
   * @param y y coordinate of 3d cell index
   * @param z z coordinate of 3d cell index
   * @return true if the particle is at the edge of the domian
   */
  bool isBorderCell(int x, int y, int z) {
    return (x == 1 || y == 1 || z == 1 || x == numCellsX - 2 || y == numCellsY - 2 || z == numCellsZ - 2);
  }

  /**
   * @brief Adds ghost particles to the ghost cells adjacent to the reflective borders of the current border cell
   * @param particle Particle for which we have to create ghost particles
   * @param cell_index Index to the cell the particle is located in
   * @param cell Reference of the cell the particle is located in
   */
  void createGhostParticles(Particle &particle, const int cell_index, Cell &cell);

  /**
   * @brief Creates ghost particles for all particles located in border cells and creates pointers to acces them
   */
  void updateGhost();

  /**
   * @brief Initializes the particles with the brownian motion
   */
  void initializeBrownianMotion();
};
