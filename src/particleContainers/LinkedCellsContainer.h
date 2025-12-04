//
// Created by jannik on 12/3/25.
//
#pragma once
#include <functional>

#include "../LinkedCells/Cell.h"
#include "ParticleContainerV2.h"
class LinkedCellsContainer : public ParticleContainerV2 {
  const double epsilon = 5;
  const double sigma = 1;
  const double cutoff_radius = 3.0;
  const double repulsing_distance;
  const double delta_t;
  bool is2D;
  /**
   * 1-D Array with all Cells
   */
  std::vector<Cell> cells;
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
  /**
Describes how many cells the overall structure has in Z-direction   */
  // int numCellsZ;
  //  Für die Höhe, Länge und Breite einer einzelnen Zelle. Es kann sein, das wir die Info aber garnicht brauchen
  double cellSizeX;
  double cellSizeY;
  double cellSizeZ;

  /**
   * Finds the neighbour-cells of the given cell and returns their cell-array indexes
   * @param cellIndex 1D cell index of the current cell
   * @returns array of 1D cell indexes
   */
  std::array<int, 26> getNeighbourCells(int cellIndex) { return cells[cellIndex].neighbors; }

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
   * @brief Moves the particles that left a cell into their new cell
   */
  void moveParticles();

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

 public:
  /**
   * Initializes the variables and cells with their cell-type and adds the respective particles to the cell
   * @param size_x domain size in x direction
   * @param size_y domain size in y direction
   * @param size_z domain size in z direction
   * @param cutoffRadius cutoff radius set in the simluation
   */
  LinkedCellsContainer(std::vector<Particle> &particles, Vector3 domain, double cutoffRadius, double delta_t,
                       std::array<BorderType, 6> borders = {OUTFLOW}, bool is2D = false);

  void applyToAllPairs(std::function<Vector3(Particle &p1, Particle &p2)> calculateForce) override;
  void applyToAllParticles(std::function<Vector3(Particle &)> apply, int k) override;
  void updatePosition(Vector3 calculateX(Particle &p, double delta_t)) override;
  void updateVelocity(Vector3 calculateV(Particle &p, double delta_t)) override;
};
