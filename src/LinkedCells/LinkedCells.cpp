//
// Created by jv_fedora on 19.11.25.
//

#include "LinkedCells.h"

#include "spdlog/fmt/bundled/base.h"

LinkedCells::LinkedCells(Vector3 lower_left_front_corner) {
   // @TODO
 }
void LinkedCells::getNeighbourCells(int cellIndex, std::array<Cell, 9> &neighbourCells) {
  std::array<int, 2> koordinates;
  getCellKoordinates(cellIndex, koordinates);
  int index = 0;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      int currentCellIndex = getCellIndex(koordinates[0]+i, koordinates[1] + j);
      neighbourCells[index] = cells[currentCellIndex];
      index++;
    }
  }
 }

void LinkedCells::getCellKoordinates(int cellIndex, std::array<int, 2> koordinates) {
  koordinates[0] = cellIndex % numCellsX;
  koordinates[1] = cellIndex / numCellsX;
}


int LinkedCells::getCellIndex(int x, int y) {
   return numCellsX * y + x; //Breite einer Zeile *  Anzahl der Zeilen über der aktuellen (wir fangen bei 0 an zu zählen) + die anzahl der Zellen in der aktuellen Zeile
 }

void LinkedCells::getNeighbours(int cellIndex, ParticleContainer &neighbourParticles) {
  std::array<Cell, 9> neighbourCells;
  getNeighbourCells(cellIndex, neighbourCells);
  for (int i = 0; i < neighbourCells.size(); i++) {
    neighbourParticles.particles.insert(neighbourParticles.particles.end(), neighbourCells[i].getParticleContainer().particles.begin(), neighbourCells[i].getParticleContainer().particles.end());
  }
}



