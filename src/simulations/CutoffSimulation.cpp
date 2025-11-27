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
  updateX();
  updateF();
  updateV();
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
    Cell &cell = linkedCells.cells[i];

    for (int j = 0; j < cell.particles.size(); j++) {
      auto p = cell.particles[j];

      const int k = linkedCells.coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]

      spdlog::trace("Moving particle from cell {} to cell {}", k, i);
      Cell &newCell = linkedCells.cells[k];
      if (newCell.cell_type != CellType::GHOST)
        newCell.particles.push_back(p);
      else
        p->setType(-1);  // mark particle as dead

      // erase p from cell[i]
      cell.particles[j] = cell.particles.back();
      cell.particles.pop_back();

      j--;
    }
  }
}

void CutoffSimulation::initializeBrownianMotion() {
  for (auto &p : linkedCells.particles) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, 2));
  }
}
