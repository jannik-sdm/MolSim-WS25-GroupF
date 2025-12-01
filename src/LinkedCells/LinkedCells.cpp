//
// Created by jv_fedora on 19.11.25.
//

#include "LinkedCells.h"

#include <spdlog/spdlog.h>

LinkedCells::LinkedCells(std::vector<Particle> &particles, const Vector3 domain, const double cutoff,
                         std::array<BorderType, 6> border)
    : particles(particles), domain_size(domain) {
  // calculate number of cells
  numCellsX = (domain_size[0] + cutoff - 1) / cutoff;
  numCellsY = (domain_size[1] + cutoff - 1) / cutoff;
  numCellsZ = (domain_size[2] + cutoff - 1) / cutoff;

  // calculate cell-size dimensions
  cellSizeX = domain_size[0] / numCellsX;
  cellSizeY = domain_size[1] / numCellsY;
  cellSizeZ = domain_size[2] / numCellsZ;
  cell_size = {cellSizeX, cellSizeY, cellSizeZ};

  // Reserve space for ghost cells
  numCellsX += 2;
  numCellsY += 2;
  numCellsZ += 2;

  cells.resize(numCellsX * numCellsY * numCellsZ);

  for (int i = 0; i < cells.size(); i++) {
    // check if cell should be ghost cell
    auto [x, y, z] = index1dToIndex3d(i);
    if (x == 0 || y == 0 || z == 0 || x == numCellsX - 1 || y == numCellsY - 1 || z == numCellsZ - 1) {
      cells[i].cell_type = CellType::GHOST;
    }
    // check if cell should be a border cell
    else {
      setNeighbourCells(i);
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

  int index = 0;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      for (int k = -1; k < 2; k++) {
        if (i == 0 && j == 0 && k == 0) continue;

        const int currentCellIndex = index3dToIndex1d(coordinates[0] + i, coordinates[1] + j, coordinates[2] + k);
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
