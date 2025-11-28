//
// Created by jannik on 11/23/25.
//
#include "CutoffSimulation.h"

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "../utils/ArrayUtils.h"
#include "../utils/MaxwellBoltzmannDistribution.h"
#include "Physics.h"

CutoffSimulation::CutoffSimulation(std::vector<Particle> &particles, Vector3 dimension, double end_time, double delta_t,
                                   double cutoffRadius)
    : end_time(end_time),
      delta_t(delta_t),
      cutoffRadius(cutoffRadius),
      linkedCells(particles, dimension, cutoffRadius),
      particles(particles) {
  initializeBrownianMotion();
}

void CutoffSimulation::iteration() {
  spdlog::trace("Updating Positions");
  updateX();
  spdlog::trace("Updating Forces");
  updateF();
  spdlog::trace("Updating Velocities");
  updateV();
  spdlog::trace("Moving Particles");
  moveParticles();
}

void CutoffSimulation::updateF() {
  // set the force of all particles to zero
  for (Particle &particle : linkedCells.particles) particle.setF({0, 0, 0});

  // Calculate forces in own cell
  for (Cell &cell : linkedCells.cells) {
    for (int i = 0; i < cell.particles.size(); i++) {
      const auto p1 = cell.particles[i];

      for (int j = i + 1; j < cell.particles.size(); j++) {
        const auto p2 = cell.particles[j];

        if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
        p1->addF(f);
        p2->subF(f);
      }
    }
  }

  // Calculate forces with neighbour cells
  for (int i = 0; i < linkedCells.cells.size(); i++) {
    auto &c1 = linkedCells.cells[i];
    // if (c1.cell_type == CellType::GHOST) continue;
    std::array<int, 26> neighbourCellsIndex = linkedCells.getNeighbourCells(i);
    for (const int j : neighbourCellsIndex) {
      if (j < i) continue;  // Skip same pairs

      auto &c2 = linkedCells.cells[j];

      for (const auto p1 : c1.particles) {
        for (const auto p2 : c2.particles) {
          if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

          Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
          // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
          p1->addF(f);
          p2->subF(f);
        }
      }
    }
  }
}

void CutoffSimulation::updateX() {
  for (auto &particle : linkedCells.particles) {
    // skip dead particles
    if (particle.getType() < 0) continue;
    particle.setX(Physics::calculateX(particle, delta_t));
  }
}

void CutoffSimulation::updateV() {
  for (auto &particle : linkedCells.particles) {
    if (particle.getType() < 0) continue;
    particle.setV(Physics::calculateV(particle, delta_t));
  }
}

void CutoffSimulation::moveParticles() {
  for (int i = 0; i < linkedCells.cells.size(); i++) {
    Cell &current_cell = linkedCells.cells[i];

    for (int j = 0; j < current_cell.particles.size(); j++) {
      auto p = current_cell.particles[j];

      const int k = linkedCells.coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]

      spdlog::trace("Moving particle from cell {} to cell {}", k, i);
      Cell &new_cell = linkedCells.cells[k];
      if (new_cell.cell_type != CellType::GHOST) {
        new_cell.particles.push_back(p);

        if (current_cell.cell_type == CellType::REGULAR && new_cell.cell_type == CellType::BORDER) {
          // particle leaves a regular cell and enters a border cell ==> create ghostParticles for each reflection
          // border
          createGhostParticles(j, current_cell, k, new_cell);
        }
      } else if (current_cell.cell_type == CellType::GHOST && new_cell.cell_type == CellType::REGULAR) {
        // particle leaves a border cell and enters a regular cell
        // TODO: delete all ghost cells associated with this particle
      } else
        p->setType(-1);  // mark particle as dead

      // erase p from cell[i]
      current_cell.particles[j] = current_cell.particles.back();
      current_cell.particles.pop_back();

      j--;
    }
  }
}

void CutoffSimulation::createGhostParticles(int particle_index, Cell &current_cell, const int current_cell_index,
                                            Cell &new_cell) {
  for (int l = 0; l < 6; l++) {
    if (new_cell.borders[l] != REFLECTION) continue;
    // create ghost particles for every reflection border

    // X of ghost Particle
    Vector3 ghostParticleX = current_cell.particles[particle_index]->getX();
    double deltaBorder = linkedCells.getBorderDistance(current_cell_index, l, ghostParticleX);
    ghostParticleX[l % 3] += (l < 3) ? -2 * deltaBorder : 2 * deltaBorder;
    // V of ghost Particle
    Vector3 ghostParticleV = current_cell.particles[particle_index]->getV();
    ghostParticleV[l % 3] *= -1;
    // Save ghost Particle
    int ghostCellIndex1d = linkedCells.coordinate3dToIndex1d(ghostParticleX);
    spdlog::trace("Adding Ghost Particle with coordinates ({}, {}, {}) to Cell with index {}/{}", ghostParticleX[0],
                  ghostParticleX[1], ghostParticleX[2], ghostCellIndex1d, linkedCells.cells.size());
    linkedCells.cells[ghostCellIndex1d].ghostParticles.push_back(
        Particle(ghostParticleX, ghostParticleV, current_cell.particles[particle_index]->getM(), 0));
    linkedCells.cells[ghostCellIndex1d].particles.push_back(&linkedCells.cells[ghostCellIndex1d].ghostParticles.back());
  }
}

void CutoffSimulation::initializeBrownianMotion() {
  for (auto &p : linkedCells.particles) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, 2));
  }
}
