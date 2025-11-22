//
// Created by jv_fedora on 19.11.25.
//

#include <vector>
#include "Cell.h"
#include "ParticleContainer.h"
#include <array>

class LinkedCells {
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
Describes how many cells the overall structure has in Z-direction   */
//int numCellsZ;
  // Für die Höhe, Länge und Breite einer einzelnen Zelle. Es kann sein, das wir die Info aber garnicht brauchen
  double IndividualX;
  double IndividualY;
  // double IndividualZ;//

  /**
   * Determines the dimensions of one the cells in the simulation based on the cutoff radius
   * and the domain size, such that the cells fit into the domain without a remainder.
   * @param cutoff cutoff-radius set in the simulation
   * @return array of size 3: <size_x, size_y, size_z>
   */
  std::array<double, 3> calculateCellSize(double cutoff);

  /**
   * Calculates the 1D CellIndex of a 3D Position
   * @param x x-position of the Cell
   * @param y y-position of the Cell
   * @param y z-position of the Cell
   * @return CellIndex
   */
  int getCellIndex(double x, double y, double z);
  /**
   * Fills the parameter coordinates with the 2D-coordinates, of a 1D CellIndex
   * @param cellIndex CellIndex in a 1D Array
   * @param coordinates address to return x and y coordinates of this cell in a 2D array
   */
  void getCellCoordinates(int cellIndex, std::array<int, 2> &coordinates);

  public:
  /**
   * constructs an object to contain the cells
   * @param size_x domain size in x direction
   * @param size_y domain size in y direction
   * @param size_z domain size in z direction
   * @param cutoff cutoff radius set in the simluation
   */
  LinkedCells(double size_x, double size_y, double size_z, double cutoff) {
    domain_size[0] = size_x;
    domain_size[1] = size_y;
    domain_size[2] = size_z;
    cell_size = calculateCellSize(cutoff);
  }
   /**
    * Finds the neighbour Cells of the given Cell and stores them in neighbourCells
    * @param cellIndex
    * @param neighbourCells
    */
  void getNeighbourCells(int cellIndex, std::array<Cell, 9> &neighbourCells);
  void getNeighbourParticles(int cellIndex, ParticleContainer &neighbourParticles);

};