//
// Created by jv_fedora on 19.11.25.
//

#include "LinkedCells.h"

#include <spdlog/spdlog.h>

LinkedCells::LinkedCells(std::vector<Particle> &particles, const Vector3 domain, const double cutoff,
                         std::array<BorderType, 6> border)
    : particles(particles), domain_size(domain) {
  // calculate number of cells - should always be at least 1
  numCellsX = std::max(1, static_cast<int>(domain_size[0] / cutoff));
  numCellsY = std::max(1, static_cast<int>(domain_size[1] / cutoff));
  numCellsZ = std::max(1, static_cast<int>(domain_size[2] / cutoff));

  // calculate cell-size dimensions
  cellSizeX = domain_size[0] / numCellsX;
  cellSizeY = domain_size[1] / numCellsY;
  cellSizeZ = domain_size[2] / numCellsZ;
  cell_size = {cellSizeX, cellSizeY, cellSizeZ};

  // Reserve space for ghost cells
  numCellsX += 2;
  numCellsY += 2;
  numCellsZ += 2;
  numCells = {numCellsX, numCellsY, numCellsZ};

  cells.resize(numCellsX * numCellsY * numCellsZ);

  for (int i = 0; i < cells.size(); i++) {
    // check if cell should be ghost cell
    auto [x, y, z] = index1dToIndex3d(i);
    if (x == 0 || y == 0 || z == 0 || x == numCellsX - 1 || y == numCellsY - 1 || z == numCellsZ - 1) {
      cells[i].cell_type = CellType::GHOST;
    }
    // check if cell should be a border cell
    else {
      if (x == 1 || y == 1 || z == 1 || x == numCellsX - 2 || y == numCellsY - 2 || z == numCellsZ - 2) {
        cells[i].cell_type = CellType::BORDER;
        // Set Border Types
        if (x == 1) {
          cells[i].borders[0] = border[0];
        }
        if (y == 1) {
          cells[i].borders[1] = border[3];
        }
        if (z == 1) {
          cells[i].borders[2] = border[1];
        }
        if (x == numCellsX - 2) {
          cells[i].borders[3] = border[4];
        }
        if (y == numCellsY - 2) {
          cells[i].borders[4] = border[2];
        }
        if (z == numCellsZ - 2) {
          cells[i].borders[5] = border[5];
        }
      }
      // remaining cells are REGULAR by default
      setNeighbourCells(i);
    }
  }

  // add particles to the correct cell
  for (auto &p : particles) {
    auto [x, y, z] = p.getX();

    const int cellIndex = coordinate3dToIndex1d(x, y, z);

    if (cellIndex < 0 || cellIndex >= cells.size()) {
      spdlog::error("Particle ({},{},{}) out of domain", x, y, z);
      continue;
    }
    spdlog::debug("Particle with coordinates: ({} {} {}) added to cell {}/{}", x, y, z, cellIndex, cells.size());
    cells[cellIndex].particles.push_back(&p);
  }
}

void LinkedCells::setNeighbourCells(const int cellIndex) {
  const std::array<int, 3> coordinates = index1dToIndex3d(cellIndex);
  Cell &cell = cells[cellIndex];
  int index = 0;
  std::array<bool, 6> periodic;
  std::array<int, 3> dimension = {-1, -1, -1};
  for (; dimension[0] < 2; dimension[0]++) {
    periodic[0] = (cell.borders[0] == PERIODIC && dimension[0] == -1);
    periodic[3] = (cell.borders[3] == PERIODIC && dimension[0] == 1);
    for (; dimension[1] < 2; dimension[1]++) {
      periodic[1] = (cell.borders[1] == PERIODIC && dimension[1] == -1);
      periodic[4] = (cell.borders[4] == PERIODIC && dimension[1] == 1);
      for (; dimension[2] < 2; dimension[2]++) {
        periodic[2] = (cell.borders[2] == PERIODIC && dimension[2] == -1);
        periodic[5] = (cell.borders[5] == PERIODIC && dimension[2] == 1);
        if (dimension[0] == 0 && dimension[1] == 0 && dimension[2] == 0) continue;
        //Set Periodic neighbours different, if boundarys are periodic:
        std::array<int, 3> neighbourCoordinates = coordinates;
        for (int i = 0; i < 3; i++) {
          if (periodic[i]) {
            //x/y/z Coordinate = max Zeile
            neighbourCoordinates[i] = numCells[i] -2;//NumCellsX/Y/Z
          }else if (periodic[i+3]) {
            //x/y/z Coordinate = min Zeile
            neighbourCoordinates[i] = 1;
          }else {
            //x/y/z Coordinate = aktuelle + i/j/k
            neighbourCoordinates[i] += dimension[i];
          }
        }
          const int currentCellIndex = index3dToIndex1d(neighbourCoordinates[0], neighbourCoordinates[1], neighbourCoordinates[2]);
          cells[cellIndex].neighbors[index] = currentCellIndex;
        index++;
      }
    }
  }
}

std::array<int, 3> LinkedCells::index1dToIndex3d(const int cellIndex) {
  std::array<int, 3> coordinates;
  const int rem = cellIndex % (numCellsX * numCellsY);

  coordinates[0] = rem % numCellsX;
  coordinates[1] = rem / numCellsX;
  coordinates[2] = cellIndex / (numCellsX * numCellsY);

  return coordinates;
}

int LinkedCells::index3dToIndex1d(const int x, const int y, const int z) {
  return x + numCellsX * y + numCellsX * numCellsY * z;
}

std::array<int, 3> LinkedCells::coordinate3dToIndex3d(const double x, const double y, const double z) {
  std::array<int, 3> indexes;
  indexes[0] = static_cast<int>(std::floor(x / cellSizeX)) + 1;
  indexes[1] = static_cast<int>(std::floor(y / cellSizeY)) + 1;
  indexes[2] = static_cast<int>(std::floor(z / cellSizeZ)) + 1;
  return indexes;
}

int LinkedCells::coordinate3dToIndex1d(const double x, const double y, const double z) {
  std::array<int, 3> index3d = coordinate3dToIndex3d(x, y, z);
  return index3dToIndex1d(index3d[0], index3d[1], index3d[2]);
}

double LinkedCells::getBorderDistance(const int cellIndex, const int border, Vector3 pos) {
  // cellIndex is not needed, but since every function which calls this shoould have cellIndex, we can pass it, because
  // it seems more efficient than calculating it to get 3d index of the cell
  std::array<int, 3> cellIndex3d = this->index1dToIndex3d(cellIndex);
  // border 0, 3 -> x-direction, border 1,4 -> y-direction, border 2,5 -> z-direction
  int axis = border % 3;
  // calculate position of the border wall (cellIndex -1, because coordinates (0,0,0) belong to cell with index3d
  // (1,1,1)) add length of cells up to the the specified cells from either left to right, bottom to up or front to back
  double borderWall = (cellIndex3d[axis] - 1) * cell_size[axis];
  // 0,1,2 -> min-border, 3,4,5 -> max-border
  // add length of the current cell if the back, up or right border is calculated
  borderWall += (border < 3) ? 0 : cell_size[axis];
  spdlog::trace("Border Distance: {}, cellIndex3d: {}, cell_size: {}, border: {}", pos[axis] - borderWall,
                cellIndex3d[axis], cell_size[axis], border);
  return std::abs(pos[axis] - borderWall);
}

//Diese funktion funktioniert noch nicht 100 % richtig, wenn ein Partikel die Domäne verlassen möchte und es mehrere verschiedene Boundary Typen gibt
int LinkedCells::getSharedBorder(int ownIndex1d, int otherIndex1d) {
  std::array<int, 26> &neighbours = cells[ownIndex1d].neighbors;
  int i = -1;
  // Get neighbour Index
  for (; i < 26; i++) {
    if (neighbours[i] == otherIndex1d) break;
  }
  // Find Border:
  if (i < 0) {
    //Check for Periodic borders
    if (cells[otherIndex1d].cell_type == GHOST
      && std::find(cells[ownIndex1d].borders.begin(), cells[ownIndex1d].borders.end(), PERIODIC) != cells[ownIndex1d].borders.end()) {
      //Find real cell to a ghost cell
      std::array<int, 3> otherIndex3d  = index1dToIndex3d(otherIndex1d);
      for (int j = 0; j < 3; j++) {
        if (otherIndex3d[j] == numCells[j] -1) {
          otherIndex3d[j] = 1;
        }else if (otherIndex3d[j] == 0) {
          otherIndex3d[j] = numCells[j]-2;
        }
      }
      spdlog::warn("Entered recursive part");
      return getSharedBorder(ownIndex1d, index3dToIndex1d(otherIndex3d[0], otherIndex3d[1], otherIndex3d[2]));
    }
    return -1;
  }
  if (i < 9) return 0;    // All Neighbours at x = 0 Border (back)
  if (i < 12) return 1;   // All Neighbours at y = 0 Border (bottom)
  if (i == 12) return 2;  // Neighbour at z = 0 Border (right)
  if (i == 13) return 5;  // Neighbour at z = 1 Border (left)
  if (i < 17) return 4;   // All Neighbours at y = 1 Border (top)
  return 3;               // All Neighbours at x = 1 Border (front)
}
