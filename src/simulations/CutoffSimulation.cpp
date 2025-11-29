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
      particles(particles),
      repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma) {
  initializeBrownianMotion();
}

void CutoffSimulation::iteration() {
  spdlog::trace("Updating Positions");
  updateX();
  spdlog::trace("Updating Ghost Particles");
  updateGhost();
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

    // skip ghost cells
    if (c1.cell_type == CellType::GHOST) continue;
    std::array<int, 26> neighbourCellsIndex = linkedCells.getNeighbourCells(i);
    for (const int j : neighbourCellsIndex) {
      if (j < i) continue;  // Skip same pairs

      auto &c2 = linkedCells.cells[j];

      for (const auto p1 : c1.particles) {
        // iterate over ghost particles if c2 is a ghost cell, else use normale particles
        if (c2.cell_type == CellType::GHOST) {
          for (const auto p2 : c2.ghost_pointers) {
            const double distance = ArrayUtils::L2Norm(p1->getX() - p2->getX());
            // for ghost particles the force should only be computed if its repulsing
            // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
            if (distance >= this->repulsing_distance || distance > cutoffRadius) continue;

            Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
            // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
            p1->addF(f);
            p2->subF(f);
          }
        } else {
          // case for regular cells
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

void CutoffSimulation::updateGhost() {
  for (int cell_index = 0; cell_index < linkedCells.cells.size(); cell_index++) {
    auto &cell = linkedCells.cells[cell_index];
    if (cell.cell_type != CellType::BORDER) {
      continue;
    }

    // clear ghost_particles and ghost_pointers of the cell
    cell.size_ghost_particles = 0;
    cell.ghost_pointers
        .clear();  // dont need to free because the pointers point to the ghost particles handled in the vector

    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }

    // update the pointers to the ghost particles in the cell
    for (auto &ghost_particle : cell.ghost_particles) {
      cell.ghost_pointers.push_back(&ghost_particle);
    }
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

void CutoffSimulation::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6 ; l++) {// && l != 2 && l != 5
    if (cell.borders[l] != REFLECTION) continue;
    // create ghost particles for every reflection border

    // X of ghost Particle
    Vector3 ghostParticleX = particle.getX();

    // calculate the distance of the particle to the border l of the new cell it is moving into
    double deltaBorder = linkedCells.getBorderDistance(cell_index, l, ghostParticleX);
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
    int ghostCellIndex1d = linkedCells.coordinate3dToIndex1d(ghostParticleX);
    Cell &ghost_cell = linkedCells.cells[ghostCellIndex1d];
    int index_ghost_particle = ghost_cell.size_ghost_particles;
    spdlog::info("Adding Ghost Particle with coordinates ({}, {}, {}) to Cell with index {}/{}", ghostParticleX[0],
                 ghostParticleX[1], ghostParticleX[2], ghostCellIndex1d, linkedCells.cells.size());

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

void CutoffSimulation::initializeBrownianMotion() {
  for (auto &p : linkedCells.particles) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, 2));
  }
}
