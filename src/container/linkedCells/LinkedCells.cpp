//
// Created by jv_fedora on 19.11.25.
//

#include "container/linkedCells/LinkedCells.h"

#include <spdlog/spdlog.h>

#include "simulations/Physics.h"
#include "utils/ArrayUtils.h"

LinkedCells::LinkedCells(std::vector<Particle> &particles, const Vector3 domain, const double cutoff, bool is2D,
                         std::array<BorderType, 6> borders)
    : particles(particles), domain_size(domain), is2D(is2D) {
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
      ghostCells.push_back(i);
      cells[i].ghost_particles.resize(32);
    } else {
      setNeighbourCells(i);
      // check if cell should be a border cell
      if (isBorderCell(x, y, z)) {
        cells[i].cell_type = CellType::BORDER;
        borderCells.push_back(i);
      } else {
        cells[i].cell_type = CellType::REGULAR;
        innerCells.push_back(i);
      }
    }
    // Set Border Types
    // Set Borders also for Ghost Cells
    if (x == 1) {
      cells[i].borders[0] = borders[0];
    }
    if (y == 1) {
      cells[i].borders[1] = borders[1];
    }
    if (z == 1) {
      cells[i].borders[2] = borders[2];
    }
    if (x == numCellsX - 2) {
      cells[i].borders[3] = borders[3];
    }
    if (y == numCellsY - 2) {
      cells[i].borders[4] = borders[4];
    }
    if (z == numCellsZ - 2) {
      cells[i].borders[5] = borders[5];
    }
  }

  // add particles to the correct cell
  for (auto &p : particles) {
    auto [x, y, z] = p.getX();

    const int cellIndex = coordinate3dToIndex1d(x, y, z);

    if (cellIndex < 0 || cellIndex >= cells.size()) {
      SPDLOG_ERROR("Particle ({},{},{}) out of domain", x, y, z);
      continue;
    }
    SPDLOG_DEBUG("Particle with coordinates: ({} {} {}) added to cell {}/{}", x, y, z, cellIndex, cells.size());
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
  SPDLOG_TRACE("Border Distance: {}, cellIndex3d: {}, cell_size: {}, border: {}", pos[axis] - borderWall,
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
      if (p->getState() < 0) continue;
      const int k = coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]

      SPDLOG_TRACE("Moving particle with coordinate ({},{},{}) from cell {} to cell {}", p->getX()[0], p->getX()[1],
                   p->getX()[2], i, k);
      std::array<int, 3> i3D = index1dToIndex3d(i);
      SPDLOG_TRACE("Old cell: ({},{},{})", i3D[0], i3D[1], i3D[2]);
      Cell &new_cell = cells[k];
      if (new_cell.cell_type != CellType::GHOST) {
        new_cell.particles.push_back(p);
      } else {
        // get shared border current_cell, new_cell
        BorderType border = getSharedBorderType(i, k);

        if (border == BorderType::OUTFLOW) {
          p->setState(-1);  // mark particle as dead
          SPDLOG_TRACE("Particle ({},{},{}) is dead!", p->getX()[0], p->getX()[1], p->getX()[2]);
          alive_particles--;
        } else if (border == BorderType::NAIVE_REFLECTION) {
          // First go back to the Old Position and then reflect the Velocity and calculate the new Position
          // This is not acurate, because the particle is not reflected AT the border,
          int borderIndex = getSharedBorder(i, k);
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
        } else if (border == BorderType::PERIODIC) {
          Vector3 x = p->getX();
          SPDLOG_TRACE("Particle with position ({},{},{}) left domain at one side and entered it at the other side",
                       x[0], x[1], x[2]);
          for (int index = 0; index < 3; index++) {
            if (x[index] < 0) x[index] += domain_size[index];
            if (x[index] > domain_size[index]) x[index] -= domain_size[index];
          }
          p->setX(x);
          std::array<int, 3> newCellIndex3d = coordinate3dToIndex3d(x[0], x[1], x[2]);
          int newCellIndex1d = index3dToIndex1d(newCellIndex3d[0], newCellIndex3d[1], newCellIndex3d[2]);
          cells[newCellIndex1d].particles.push_back(p);

        } else {
          SPDLOG_ERROR("A Particle escaped from the domain, even, if it shouldn't");
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
  for (int cell_index : ghostCells) {
    auto &cell = cells[cell_index];
    cell.size_ghost_particles = 0;
  }
  for (int cell_index : borderCells) {
    auto &cell = cells[cell_index];
    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }
  }
}

void LinkedCells::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6; l++) {
    if (is2D && (l == 2 || l == 5)) continue;
    if (cell.borders[l] == BorderType::REFLECTION) {
      // create ghost particles for every reflection border

      // X of ghost Particle
      Vector3 ghostParticleX = particle.getX();
      SPDLOG_TRACE("ghostParticleX: ({},{},{})", ghostParticleX[0], ghostParticleX[1], ghostParticleX[2]);
      // calculate the distance of the particle to the border l of the new cell it is moving into
      double deltaBorder = getBorderDistance(cell_index, l, ghostParticleX);
      double particle_distance = 2 * deltaBorder;
      if (particle_distance >= calcRepulsingDistance(particle.getSigma(), particle.getSigma())) {
        // ghost particle should only be created if it is repulsing to it's respective particle
        continue;
      }
      ghostParticleX[l % 3] += (l < 3) ? -particle_distance : particle_distance;
      // V of ghost Particle
      Vector3 ghostParticleV = particle.getV();
      ghostParticleV[l % 3] *= -1;
      // Save ghost Particle
      int ghostCellIndex1d = coordinate3dToIndex1d(ghostParticleX);
      SPDLOG_TRACE(
          "Adding Ghost Particle with coordinates ({}, {}, {}) (Ghost Particle of ({},{},{}))to Cell with index {}/{}",
          ghostParticleX[0], ghostParticleX[1], ghostParticleX[2], particle.getX()[0], particle.getX()[1],
          particle.getX()[2], ghostCellIndex1d, cells.size());
      Cell &ghost_cell = cells[ghostCellIndex1d];
      int index_ghost_particle = ghost_cell.size_ghost_particles;

      SPDLOG_TRACE("deltaBorder: {} distance: {}", 2 * deltaBorder,
                   ArrayUtils::L2Norm(particle.getX() - ghostParticleX));

      if (index_ghost_particle < ghost_cell.ghost_particles.size()) {
        // vector has enough allocated space ==> reuse old particle objects
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setX(ghostParticleX);
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setV(ghostParticleV);
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setM(particle.getM());
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setEpsilon(particle.getEpsilon());
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setSigma(particle.getSigma());

      } else {
        // need to push_back new particles
        ghost_cell.ghost_particles.reserve(2 * ghost_cell.ghost_particles.size());
        ghost_cell.ghost_particles.emplace_back(ghostParticleX, ghostParticleV, particle.getM(), particle.getEpsilon(),
                                                particle.getSigma());
      }

      ghost_cell.size_ghost_particles++;
    }
    if (cell.borders[l] == BorderType::PERIODIC) {
      // Ghost Partikel Position zu der des echten Partikels berechnen
      Vector3 ghostParticleX = particle.getX();
      ghostParticleX[l % 3] += (l < 3) ? domain_size[l % 3] : -domain_size[l % 3];
      int ghostCellIndex1d = coordinate3dToIndex1d(ghostParticleX);
      SPDLOG_TRACE(
          "Adding Ghost Particle with coordinates ({}, {}, {}) (Ghost Particle of ({},{},{}))to Cell with index {}/{}",
          ghostParticleX[0], ghostParticleX[1], ghostParticleX[2], particle.getX()[0], particle.getX()[1],
          particle.getX()[2], ghostCellIndex1d, cells.size());
      Cell &ghost_cell = cells[ghostCellIndex1d];
      int index_ghost_particle = ghost_cell.size_ghost_particles;

      if (index_ghost_particle < ghost_cell.ghost_particles.size()) {
        // vector has enough allocated space ==> reuse old particle objects
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setX(ghostParticleX);
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setV(particle.getV());
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setM(particle.getM());
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setEpsilon(particle.getEpsilon());
        ghost_cell.ghost_particles[ghost_cell.size_ghost_particles].setSigma(particle.getSigma());

      } else {
        // need to push_back new particles
        ghost_cell.ghost_particles.reserve(2 * ghost_cell.ghost_particles.size());
        ghost_cell.ghost_particles.emplace_back(ghostParticleX, particle.getV(), particle.getM(), particle.getEpsilon(),
                                                particle.getSigma());
      }
      // Also Periodic Ghost can cause new Ghosts
      ghost_cell.size_ghost_particles++;
      createGhostParticles(ghost_cell.ghost_particles[index_ghost_particle], ghostCellIndex1d, ghost_cell);
    }
  }
}
int LinkedCells::getPeriodicEquivalentForGhost(const int cellIndex, const int ghostCellIndex) {
  auto borders = cells[cellIndex].borders;
  // Überblick: Welche Borders hat die GhostZelle mit der echten gemeinsam?
  auto borderTypes = getSharedBordersIndex(cellIndex, ghostCellIndex);
  std::array<int, 3> index3d = index1dToIndex3d(ghostCellIndex);
  for (int borderIndex : borderTypes) {
    // Wenn eine Border nicht periodisch ist muss an ihr auch nicht gespiegelt werden
    if (borders[borderIndex] != BorderType::PERIODIC) continue;
    int dim = borderIndex % 3;
    // Check für zusätzliche Sicherheit: War die Border wirklich zu einer Ghost Zelle
    if (index3d[dim] >= numCells[dim] - 1) {
      index3d[dim] = 1;
    } else if (index3d[dim] <= 0) {
      index3d[dim] = numCells[dim] - 2;
    }
  }
  /*if (index3d[0] == index1dToIndex3d(cellIndex)[0] && index3d[1] == index1dToIndex3d(cellIndex)[1] &&
      index3d[2] == index1dToIndex3d(cellIndex)[2]) {
    SPDLOG_ERROR("some error with calculating a periodic cell of a ghost cell");
  }*/
  return index3dToIndex1d(index3d[0], index3d[1], index3d[2]);
}

inline BorderType best_of(const std::array<BorderType, 6> &borders, const std::vector<int> &idx) {
  BorderType best = BorderType::ERROR;
  for (int i : idx) {
    best = std::max(best, static_cast<BorderType>(borders[i]));
  }
  return best;
}

const std::vector<int> LinkedCells::getSharedBordersIndex(const int ownIndex1d, const int otherIndex1d) {
  auto &ownCell = cells[ownIndex1d];
  std::array<int, 26> &neighbours = ownCell.neighbors;
  int foundIndex = -1;
  for (int i = 0; i < 26; i++) {
    if (neighbours[i] == otherIndex1d) {
      foundIndex = i;
      break;
    }
  }
  if (foundIndex < 0) {
    // Check for Periodic borders
    // Dieser Fall darf nicht vorkommen!
    /*
    if (cells[otherIndex1d].cell_type == GHOST
        // Nachsehen, ob eine der borders unserer Zelle Periodisch ist
        && std::find(cells[ownIndex1d].borders.begin(), cells[ownIndex1d].borders.end(), PERIODIC) !=
               cells[ownIndex1d].borders.end()) {
      // Find real cell to a ghost cell
      return getSharedBordersIndex(ownIndex1d, getPeriodicEquivalentForGhost(ownIndex1d, otherIndex1d));
    }*/
    SPDLOG_ERROR("Did not found other cell as neighbour of own cell");
    std::initializer_list<int> error = {-1};
    return error;
  }
  auto &borders = ownCell.borders;
  static const std::array<std::vector<int>, 26> groups = {{
      //                     x  y  z
      {0, 1, 2},  // i = 0  -1 -1 -1
      {0, 1},     // i = 1  -1 -1  0
      {0, 1, 5},  // i = 2  -1 -1  1
      {0, 2},     // i = 3  -1  0 -1
      {0},        // i = 4  -1  0  0
      {0, 5},     // i = 5  -1  0  1
      {0, 4, 2},  // i = 6  -1  1 -1
      {0, 4},     // i = 7  -1  1  0
      {0, 4, 5},  // i = 8  -1  1  1
      {1, 2},     // i = 9   0 -1 -1
      {1},        // i = 10  0 -1  0
      {1, 5},     // i = 11  0 -1  1
      {2},        // i = 12  0  0 -1
      //                       0  0  0
      {5},        // i = 13  0  0  1
      {4, 2},     // i = 14  0  1 -1
      {4},        // i = 15  0  1  0
      {4, 5},     // i = 16  0  1  1
      {3, 1, 2},  // i = 17  1 -1 -1
      {3, 1},     // i = 18  1 -1  0
      {3, 1, 5},  // i = 19  1 -1  1
      {3, 2},     // i = 20  1  0 -1
      {3},        // i = 21  1  0  0
      {3, 5},     // i = 22  1  0  1
      {3, 4, 2},  // i = 23  1  1 -1
      {3, 4},     // i = 24  1  1  0
      {3, 4, 5}   // i = 25  1  1  1
      // x=-1 -> 0, y=-1 -> 1, z=-1 -> 2, x=1 -> 3, y=1 -> 4, z=1 -> 5 für x,y,z = 0 kein Wert, weil in die Richtung ja
      // dann keine Grenze existiert
  }};
  return groups[foundIndex];
}
BorderType LinkedCells::getSharedBorderType(const int ownIndex1d, const int otherIndex1d) {
  const auto &ownCell = cells[ownIndex1d];
  return best_of(ownCell.borders, getSharedBordersIndex(ownIndex1d, otherIndex1d));
}

double LinkedCells::calcRepulsingDistance(double sigma1, double sigma2) {
  return repulsing_const * Physics::LorentzBerthelot::sigma(sigma1, sigma2);
}
