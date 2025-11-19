//
// Created by jv_fedora on 19.11.25.
//

#include <vector>
#include "Cell.h"
#include "ParticleContainer.h"

class LinkedCells {
  private:
  /**
   * 1-D Array with all Cells
   */
  std::vector<Cell> cells;
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
  // double IndividualZ;
  /**
   * Calculates the 1D CellIndex of a 2D Position
   * @param x 2D x-position of the Cell
   * @param y 2D y-position of the Cell
   * @return CellIndex in a 1D Array
   */
  int getCellIndex(int x, int y);
  /**
   * Fills the parameter koordinates with the 2D-Koordinates, of a 1D CellIndex
   * @param cellIndex CellIndex in a 1D Array
   * @param koordinates address to return x and y koordinates of this cell in a 2D array
   */
  void getCellKoordinates(int cellIndex, std::array<int, 2> koordinates);
  public:
  LinkedCells (Vector3 lower_left_front_corner);
   /**
    * Finds the neighbour Cells of the given Cell and stores the neighbours in neighbourCells
    * @param cellIndex
    * @param neighbourCells
    */
void getNeighbourCells(int cellIndex, std::array<Cell, 9> &neighbourCells);
  void getNeighbours(int cellIndex, ParticleContainer &neighbourParticles);


};