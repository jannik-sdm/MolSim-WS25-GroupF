//
// Created by jannik on 11/23/25.
//
#include "CutoffSimulation.h"

#include <unistd.h>

#include "Physics.h"
#include "../utils/ArrayUtils.h"

CutoffSimulation::CutoffSimulation(LinkedCells &linkedCells, double end_time, double delta_t, double cutoffRadius)
    : linkedCells(linkedCells), end_time(end_time), delta_t(delta_t), cutoffRadius(cutoffRadius) {}

void CutoffSimulation::iteration() {
  moveParticles();
  updateX();
  updateF();
  updateV();
}

void CutoffSimulation::updateF() {
  // set the force of all particles to zero
  for (auto particle : linkedCells.particles) particle.setF({0, 0, 0});

  // Calculate forces in own cell
  for (auto cell : linkedCells.cells) {
    for (int i = 0; i < cell.particles.size(); i++) {
      auto &p1 = *cell.particles[i];

      for (int j = i + 1; j < cell.particles.size(); j++) {
        auto &p2 = *cell.particles[j];

        if (ArrayUtils::L2Norm(p1.getX() - p2.getX()) > cutoffRadius) continue;

        Vector3 f = Physics::lennardJonesForce(p1, p2, sigma, epsilon);
        p1.addF(f);
        p2.subF(f);
      }
    }
  }

  // Calculate forces with neighbour cells
  for (int i = 0; i < linkedCells.cells.size(); i++) {
    auto c1 = linkedCells.cells[i];

    std::array<int, 26> neighbourCellsIndex = linkedCells.getNeighbourCells(i);
    for (const int j : neighbourCellsIndex) {
      if (j < i) continue; // Skip same pairs

      auto c2 = linkedCells.cells[j];

      for (const auto p1 : c1.particles) {
        for (const auto p2: c2.particles) {
          if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

          Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
          p1->addF(f);
          p2->subF(f);
        }
      }
    }
  }
}

void CutoffSimulation::updateX() {
  for (auto particle : linkedCells.particles) {
    particle.setX(Physics::calculateX(particle, delta_t));
  }
}

void CutoffSimulation::updateV() {
  for (auto particle : linkedCells.particles) {
    particle.setV(Physics::calculateV(particle, delta_t));
  }
}

void CutoffSimulation::moveParticles() {
  for (int i = 0; i < linkedCells.cells.size(); i++) {
    auto cell = linkedCells.cells[i];
    for (int j = 0; j < cell.particles.size(); j++) {
      auto p = cell.particles[j];

      const int k = linkedCells.coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]
      auto newCell = linkedCells.cells[k];
      newCell.particles.push_back(p);

      // erase p from cell[i]
      cell.particles[j] = cell.particles.back();
      cell.particles.pop_back();
    }
  }
}