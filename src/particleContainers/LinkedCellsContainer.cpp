//
// Created by jannik on 12/3/25.
//

#include "LinkedCellsContainer.h"

#include "cmath"
#include "spdlog/spdlog.h"
#include "utils/ArrayUtils.h"

LinkedCellsContainer::LinkedCellsContainer(std::vector<Particle> &particles, Vector3 domain, double cutoff_radius,
                                           double delta_t, std::array<BorderType, 6> border, bool is2D)
    : ParticleContainerV2(particles),
      domain_size(domain),
      is2D(is2D),
      repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma),
      delta_t(delta_t) {
  // calculate number of cells
  numCellsX = (domain_size[0] + cutoff_radius - 1) / cutoff_radius;
  numCellsY = (domain_size[1] + cutoff_radius - 1) / cutoff_radius;
  numCellsZ = (domain_size[2] + cutoff_radius - 1) / cutoff_radius;

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

void LinkedCellsContainer::setNeighbourCells(const int cellIndex) {
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

std::array<int, 3> LinkedCellsContainer::index1dToIndex3d(const int cellIndex) {
  std::array<int, 3> coordinates;
  const int rem = cellIndex % (numCellsX * numCellsY);

  coordinates[0] = rem % numCellsX;
  coordinates[1] = rem / numCellsX;
  coordinates[2] = cellIndex / (numCellsX * numCellsY);

  return coordinates;
}

int LinkedCellsContainer::index3dToIndex1d(const int x, const int y, const int z) {
  return x + numCellsX * y + numCellsX * numCellsY * z;
}

std::array<int, 3> LinkedCellsContainer::coordinate3dToIndex3d(const double x, const double y, const double z) {
  std::array<int, 3> indexes;
  indexes[0] = static_cast<int>(std::floor(x / cellSizeX)) + 1;
  indexes[1] = static_cast<int>(std::floor(y / cellSizeY)) + 1;
  indexes[2] = static_cast<int>(std::floor(z / cellSizeZ)) + 1;
  return indexes;
}

int LinkedCellsContainer::coordinate3dToIndex1d(const double x, const double y, const double z) {
  std::array<int, 3> index3d = coordinate3dToIndex3d(x, y, z);
  return index3dToIndex1d(index3d[0], index3d[1], index3d[2]);
}

double LinkedCellsContainer::getBorderDistance(const int cellIndex, const int border, Vector3 pos) {
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

void LinkedCellsContainer::applyToAllPairs(std::function<Vector3(Particle &p1, Particle &p2)> calculateForce) {
  // set the force of all particles to zero
  for (Particle &particle : particles) particle.setF({0, 0, 0});

  for (int c = 0; c < cells.size(); c++) {
    auto &c1 = cells[c];

    // Calculate forces in own cell
    for (int i = 0; i < c1.particles.size(); i++) {
      const auto p1 = c1.particles[i];

      for (int j = i + 1; j < c1.particles.size(); j++) {
        const auto p2 = c1.particles[j];

        if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoff_radius) continue;

        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        Vector3 f = calculateForce(*p1, *p2);
        p1->addF(f);
        p2->subF(f);
      }
    }

    // calculate forces for neighbor cells
    if (c1.cell_type == CellType::GHOST) continue;
    std::array<int, 26> neighbourCellsIndex = getNeighbourCells(c);
    for (const int j : neighbourCellsIndex) {
      auto &c2 = cells[j];
      // newton optimization, but ONLY if c2 is not a Ghost cell, because if this calculation is skipped, particles are
      // not repulsed
      if (j < c && c2.cell_type != CellType::GHOST) continue;

      for (const auto p1 : c1.particles) {
        // iterate over ghost particles if c2 is a ghost cell, else use normale particles
        if (c2.cell_type == CellType::GHOST) {
          for (int k = 0; k < c2.size_ghost_particles; k++) {
            Particle &p2 = c2.ghost_particles[k];
            const double distance = ArrayUtils::L2Norm(p1->getX() - p2.getX());
            // for ghost particles the force should only be computed if its repulsing
            // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
            spdlog::trace("reached radius check for ghost particles");
            if (distance >= this->repulsing_distance || distance > cutoff_radius) continue;

            Vector3 f = calculateForce(*p1, p2);
            p1->addF(f);
            p2.subF(f);
            // spdlog::trace("adding force of ghost particle: {} {} {}", f[0], f[1], f[2]);
          }
        } else {
          // case for regular cells
          for (const auto p2 : c2.particles) {
            if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoff_radius) continue;

            Vector3 f = calculateForce(*p1, *p2);
            p1->addF(f);
            p2->subF(f);
          }
        }
      }
    }
  }
}

void LinkedCellsContainer::applyToAllParticles(std::function<Vector3(Particle &)> apply, int k) {
  for (auto &p : particles) {
    Vector3 x = apply(p);
    if (k == 0) {
      p.setX(x);
    } else if (k == 1) {
      p.setV(x);
    } else if (k == 2) {
      p.setF(x);
    }
  }
}

void LinkedCellsContainer::updatePosition(Vector3 calculateX(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    p.setX(calculateX(p, delta_t));
  }
  updateGhost();
}

void LinkedCellsContainer::updateVelocity(Vector3 updateV(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    p.setV(updateV(p, delta_t));
  }
  moveParticles();
}

void LinkedCellsContainer::moveParticles() {
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
      } else
        p->setType(-1);  // mark particle as dead

      // erase p from cell[i] by swapping p to the back of the vector
      current_cell.particles[j] = current_cell.particles.back();
      current_cell.particles.pop_back();

      // decrement j to prevent skipping the particle moved to position j from the back
      j--;
    }
  }
}

void LinkedCellsContainer::updateGhost() {
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
    if (cell.cell_type != BORDER) continue;
    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }
  }
}

void LinkedCellsContainer::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6; l++) {
    if (is2D && (l == 2 || l == 5)) continue;
    if (cell.borders[l] != REFLECTION) continue;

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
