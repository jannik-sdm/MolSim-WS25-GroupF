//
// Created by jv_fedora on 19.11.25.
//

#include "LinkedCells.h"

std::array<double, 3> LinkedCells::calculateCellSize(double cutoff) {
  numCellsX = domain_size[0] / cutoff;
  numCellsY = domain_size[1] / cutoff;
  // numCellsZ = domain_size[2] / cutoff;

  // TODO: change z length from zero to real length
  return {domain_size[0] / numCellsX, domain_size[1] / numCellsY, 0};
}

void LinkedCells::getNeighbourCells(int cellIndex, std::array<Cell, 9> &neighbourCells) {
  std::array<int, 2> coordinates;
  getCellCoordinates(cellIndex, coordinates);
  int index = 0;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      int currentCellIndex = getCellIndex(coordinates[0]+i, coordinates[1] + j, 0);
      neighbourCells[index] = cells[currentCellIndex];
      index++;
    }
  }
 }

void LinkedCells::getCellCoordinates(int cellIndex, std::array<int, 2> &coordinates) {
  coordinates[0] = cellIndex % numCellsX;
  coordinates[1] = cellIndex / numCellsX;
}

int LinkedCells::getCellIndex(double x, double y, double z) {
  // number of cells per line * how many lines we skipped in y direction + index in x direction
   return numCellsX * (y / cell_size[1]) + (x / cell_size[0]);
 }
