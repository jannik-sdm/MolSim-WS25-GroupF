//
// Created by jv_fedora on 19.11.25.
//

#include "container/linkedCells/LinkedCells.h"

#include <spdlog/spdlog.h>

#include "simulations/Physics.h"
#include "utils/ArrayUtils.h"

LinkedCells::LinkedCells(std::vector<Particle> &particles, const Vector3 domain, const double cutoff, bool is2D,
                         double repulsing_distance, std::array<BorderType, 6> borders)
    : particles(particles), domain_size(domain), is2D(is2D), repulsing_distance(repulsing_distance) {
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

  cells.resize(numCellsX * numCellsY * numCellsZ);

  for (int i = 0; i < cells.size(); i++) {
    // check if cell should be ghost cell
    auto [x, y, z] = index1dToIndex3d(i);
    if (x == 0 || y == 0 || z == 0 || x == numCellsX - 1 || y == numCellsY - 1 || z == numCellsZ - 1) {
      cells[i].cell_type = CellType::GHOST;
      continue;  // ghost cells don't need neighbours
    }

    setNeighbourCells(i);

    // check if cell should be a border cell
    if (isBorderCell(x, y, z)) {
      cells[i].cell_type = CellType::BORDER;
      // Set Border Types
      if (x == 1) {
        cells[i].borders[0] = borders[0];
      }
      if (y == 1) {
        cells[i].borders[1] = borders[3];
      }
      if (z == 1) {
        cells[i].borders[2] = borders[1];
      }
      if (x == numCellsX - 2) {
        cells[i].borders[3] = borders[4];
      }
      if (y == numCellsY - 2) {
        cells[i].borders[4] = borders[2];
      }
      if (z == numCellsZ - 2) {
        cells[i].borders[5] = borders[5];
      }

      continue;
    }

    // remaining cells are REGULAR by default
    cells[i].cell_type = CellType::REGULAR;
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
  // initialize alive particles
  for (auto &p : particles)
    if (p.getState() != -1) alive_particles++;
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

int LinkedCells::getSharedBorder(int ownIndex1d, int otherIndex1d) {
  NeighBourIndices &neighbours = cells[ownIndex1d].neighbors;
  int i = -1;
  // Get neighbour Index
  for (; i < 26; i++) {
    if (neighbours[i] == otherIndex1d) break;
  }
  // Find Border:
  if (i < 0) return -1;
  if (i < 9) return 0;    // All Neighbours at x = 0 Border (back)
  if (i < 12) return 1;   // All Neighbours at y = 0 Border (bottom)
  if (i == 12) return 2;  // Neighbour at z = 0 Border (right)
  if (i == 13) return 5;  // Neighbour at z = 1 Border (left)
  if (i < 17) return 4;   // All Neighbours at y = 1 Border (top)
  return 3;               // All Neighbours at x = 1 Border (front)
}

void LinkedCells::moveParticles() {
  for (int i = 0; i < cells.size(); i++) {
    Cell &current_cell = cells[i];

    for (int j = 0; j < current_cell.particles.size(); j++) {
      auto p = current_cell.particles[j];

      const int k = coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]

      spdlog::trace("Moving particle with coordinate ({},{},{}) from cell {} to cell {}", p->getX()[0], p->getX()[1],
                    p->getX()[2], k, i);
      Cell &new_cell = cells[k];
      if (new_cell.cell_type != CellType::GHOST) {
        new_cell.particles.push_back(p);
      } else {
        // get shared border current_cell, new_cell
        int borderIndex = getSharedBorder(i, k);
        BorderType border = current_cell.borders[borderIndex];
        if (border == BorderType::OUTFLOW) {
          p->setState(-1);  // mark particle as dead
          alive_particles--;
          spdlog::trace("Particle ({},{},{}) is dead!", p->getX()[0], p->getX()[1], p->getX()[2]);
        } else if (border == BorderType::NAIVE_REFLECTION) {
          // First go back to the Old Position and then reflect the Velocity and calculate the new Position
          // This is not acurate, because the particle is not reflected AT the border,
          Vector3 v = p->getV();
          v[borderIndex % 3] *= -1;
          Vector3 neg = {-1, -1, -1};
          p->setV(neg * p->getV());     // Turn Velocity
          Vector3 oldF = p->getOldF();  // Save OldF
          p->setF(neg * p->getF());     // Turn F
          p->setF(oldF);
          p->setF(neg * p->getF());  // Reset old Force
          p->setV(v);                // Set new Velocity
          continue;                  // Don't move the Particle into a Ghost Cell
        } else {
          spdlog::error("A Particle escaped from the domain, even, if it shouldn't");
        }
      }
      // erase p from cell[i] by swapping p to the back of the vector
      current_cell.particles[j] = current_cell.particles.back();
      current_cell.particles.pop_back();

      // decrement j to prevent skipping the particle moved to position j from the back
      j--;
    }
  }
  updateGhost();
}

void LinkedCells::updateGhost() {
  for (int cell_index = 0; cell_index < cells.size(); cell_index++) {
    auto &cell = cells[cell_index];
    if (cell.cell_type == CellType::GHOST) {
      // clear particles of ghost cells
      cell.size_ghost_particles = 0;
      continue;
    }
  }
  for (int cell_index = 0; cell_index < cells.size(); cell_index++) {
    auto &cell = cells[cell_index];
    if (cell.cell_type != CellType::BORDER) continue;
    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }
  }
}

void LinkedCells::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6; l++) {
    if (is2D && (l == 2 || l == 5)) continue;
    if (cell.borders[l] != BorderType::REFLECTION) continue;

    // create ghost particles for every reflection border

    // X of ghost Particle
    Vector3 ghostParticleX = particle.getX();
    spdlog::trace("ghostParticleX: ({},{},{})", ghostParticleX[0], ghostParticleX[1], ghostParticleX[2]);
    // calculate the distance of the particle to the border l of the new cell it is moving into
    double deltaBorder = getBorderDistance(cell_index, l, ghostParticleX);
    double particle_distance = 2 * deltaBorder;
    if (particle_distance >= repulsing_distance) {
      // ghost particle should only be created if it is repulsing to it's respective particle
      continue;
    }
    ghostParticleX[l % 3] += (l < 3) ? -particle_distance : particle_distance;
    // V of ghost Particle
    Vector3 ghostParticleV = particle.getV();
    ghostParticleV[l % 3] *= -1;
    // Save ghost Particle
    int ghostCellIndex1d = coordinate3dToIndex1d(ghostParticleX);
    spdlog::trace(
        "Adding Ghost Particle with coordinates ({}, {}, {}) (Ghost Particle of ({},{},{}))to Cell with index {}/{}",
        ghostParticleX[0], ghostParticleX[1], ghostParticleX[2], particle.getX()[0], particle.getX()[1],
        particle.getX()[2], ghostCellIndex1d, cells.size());
    Cell &ghost_cell = cells[ghostCellIndex1d];
    int index_ghost_particle = ghost_cell.size_ghost_particles;

    spdlog::trace("deltaBorder: {} distance: {}", 2 * deltaBorder,
                  ArrayUtils::L2Norm(particle.getX() - ghostParticleX));

    if (index_ghost_particle < ghost_cell.ghost_particles.size()) {
      // vector has enough allocated space ==> reuse old particle objects
      ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setX(ghostParticleX);
      ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setV(ghostParticleV);
      ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setM(particle.getM());
    } else {
      // need to push_back new particles
      ghost_cell.ghost_particles.push_back(Particle(ghostParticleX, ghostParticleV, particle.getM()));
    }

    ghost_cell.size_ghost_particles++;
  }
}