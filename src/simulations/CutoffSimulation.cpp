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
                                   double cutoffRadius, std::array<BorderType, 6> &border, bool is2D)
    : end_time(end_time),
      delta_t(delta_t),
      cutoffRadius(cutoffRadius),
      linkedCells(particles, dimension, cutoffRadius, border),
      particles(particles),
      repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma),
      is2D(is2D) {
  initializeBrownianMotion();
}

void CutoffSimulation::iteration() {
  spdlog::debug("Updating Positions");
  updateX();
  spdlog::debug("Moving Particles");
  moveParticles();
  spdlog::debug("Updating Ghost Particles");
  updateGhost();
  spdlog::debug("Updating Forces");
  updateF();
  spdlog::debug("Updating Velocities");
  updateV();
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
        if (p1 == p2) {
          continue;
        }
        if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        if (p1->getX()[0] == p2->getX()[0] && p1->getX()[1] == p2->getX()[1]) {
          spdlog::error("HILFE");  // Dieser Punkt wird erreicht!
        }
        Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        // spdlog::info("F: {} {} {}", f[0], f[1], f[2]);
        p1->addF(f);
        p2->subF(f);
        if (f[0] > 10000 || f[1] > 10000 || f[2] > 10000) {
          spdlog::warn("High forces!");
        }
        spdlog::trace("Adding Force: ({},{},{})", f[0], f[1], f[2]);
        spdlog::trace("new Force own Cell: ({},{},{}) with particles: p1: ({},{},{}), p2: ({},{},{})", p1->getF()[0],
                      p1->getF()[1], p1->getF()[2], p1->getX()[0], p1->getX()[1], p1->getX()[2], p2->getX()[0],
                      p2->getX()[1], p2->getX()[2]);
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
      auto &c2 = linkedCells.cells[j];
      // newton optimization, but ONLY if c2 is not a Ghost cell, because if this calculation is skipped, particles are
      // not repulsed
      if (j < i && c2.cell_type != CellType::GHOST) continue;

      for (const auto p1 : c1.particles) {
        if (p1->getX()[0] < 0 || p1->getX()[1] < 0 || p1->getX()[2] < 0) {
          spdlog::error("some error with the cell labeling");
        }
        // iterate over ghost particles if c2 is a ghost cell, else use normale particles
        if (c2.cell_type == CellType::GHOST) {
          // Herausfinden, ob Periodic, oder Reflective handling. Alle anderen haben keine Partikel in Ghost Zellen
          BorderType border = linkedCells.getSharedBorderType(i, j);
          if (border == ERROR) {
            spdlog::error("Wrong Border");
            continue;
          }  // Fehlerbehandlung
          if (border == PERIODIC) { /*
             //Alternative Implementierung: Die andere Variante scheint aber stabiler zu sein
             //partikel p1 in die nähe des Periodic neighbours schieben
             Vector3 tmp = p1->getX();
             Vector3 x = tmp;
             if (border < 3) {
               x[border%3] += linkedCells.domain_size[border%3];
             }else {
               x[border%3] -= linkedCells.domain_size[border%3];
             }
             p1->setX(x);
             // c2 von der Ghost Zelle auf die Gegenüberliegende Border Zelle verschieben
             c2 = linkedCells.cells[linkedCells.getPeriodicEquivalentForGhost(j)];
             for (const auto p2 : c2.particles) {
               if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;
               if (p1->getX()[0] == p2->getX()[0] && p1->getX()[1] == p2->getX()[1]) {
                 spdlog::error("HILFE");
               }
               Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
               spdlog::trace("Adding ({},{},{}) to the force of Particle: ({},{},{})", f[0], f[1], f[2], p1->getX()[0],
             p1->getX()[1], p1->getX()[2]); p1->addF(f);
               //p2->subF(f);
               spdlog::trace("new Force Normal Cell: ({},{},{})", p1->getF()[0], p1->getF()[1], p1->getF()[2]);
             }
             p1->setX(tmp);
             */
            // Echte Zelle zu Ghost Zelle finden
            int realCellIndex = linkedCells.getPeriodicEquivalentForGhost(j);
            auto &realCell = linkedCells.cells[realCellIndex];
            // N3
            if (realCellIndex < i) continue;
            for (const auto p2 : realCell.particles) {
              if (p1 == p2) continue;  // Nur zur Sicherheit

              Vector3 diff = p2->getX() - p1->getX();
              // Kürzesten Weg zwischen zwei Zellen ermitteln
              for (int dim = 0; dim < 3; dim++) {
                if (diff[dim] > linkedCells.domain_size[dim] * 0.5) {
                  diff[dim] -= linkedCells.domain_size[dim];
                } else if (diff[dim] <= -linkedCells.domain_size[dim] * 0.5) {
                  diff[dim] += linkedCells.domain_size[dim];
                }
              }

              double distance = ArrayUtils::L2Norm(diff);

              if (distance <= cutoffRadius && distance > 0) {
                Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon, diff);

                p1->addF(f);
                p2->subF(f);
              }
            }
          }
          if (border == REFLECTION) {
            for (int k = 0; k < c2.size_ghost_particles; k++) {
              Particle &p2 = c2.ghost_particles[k];
              const double distance = ArrayUtils::L2Norm(p1->getX() - p2.getX());
              // for ghost particles the force should only be computed if its repulsing
              // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
              spdlog::trace("reached radius check for ghost particles");
              if (distance >= this->repulsing_distance || distance > cutoffRadius) continue;

              Vector3 f = Physics::lennardJonesForce(*p1, p2, sigma, epsilon);
              p1->addF(f);
              spdlog::trace("adding force of ghost particle: {} {} {}", f[0], f[1], f[2]);
              spdlog::trace("new Force Ghost Cell: ({},{},{})", p1->getF()[0], p1->getF()[1], p1->getF()[2]);
              // dont need to subtract force of ghost particles, since they are updated after anyways
            }
          }
        } else {
          // case for regular cells
          for (const auto p2 : c2.particles) {
            if (*p1 == *p2) continue;
            if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;
            //Kontroll Checks: kann entfernt werden, sobald das Programm wieder funktioniert
            if (p1->getX()[0] == p2->getX()[0] && p1->getX()[1] == p2->getX()[1]) {
              spdlog::error("HILFE");
            }
            Vector3 f = Physics::lennardJonesForce(*p1, *p2, sigma, epsilon);
            spdlog::trace("Adding ({},{},{}) to the force of Particle: ({},{},{})", f[0], f[1], f[2], p1->getX()[0],
                          p1->getX()[1], p1->getX()[2]);
            p1->addF(f);
            p2->subF(f);
            spdlog::trace("new Force Normal Cell: ({},{},{})", p1->getF()[0], p1->getF()[1], p1->getF()[2]);
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
    // spdlog::trace("Updating X:");
    // spdlog::trace("-> Old Position: ({},{},{})", particle.getX()[0], particle.getX()[1], particle.getX()[2]);
    // spdlog::trace("-> Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
    // spdlog::trace("-> Force: ({},{},{})", particle.getF()[0], particle.getF()[1], particle.getF()[2]);
    particle.setX(Physics::calculateX(particle, delta_t));
    //Kontroll Checks: kann entfernt werden, sobald das Programm wieder funktioniert
    if (particle.getX()[0] < -linkedCells.cellSizeX ||
        particle.getX()[0] > linkedCells.domain_size[0] + linkedCells.cellSizeX ||
        particle.getX()[1] < -linkedCells.cellSizeY ||
        particle.getX()[1] > linkedCells.domain_size[1] + linkedCells.cellSizeY ||
        particle.getX()[2] < -linkedCells.cellSizeZ ||
        particle.getX()[2] > linkedCells.domain_size[2] + linkedCells.cellSizeZ) {
      spdlog::error("Irgentwas stimmt nicht");
    }
    // spdlog::trace("-> New: ({},{},{})", particle.getX()[0], particle.getX()[1], particle.getX()[2]);
  }
}

void CutoffSimulation::updateV() {
  for (auto &particle : linkedCells.particles) {
    if (particle.getType() < 0) continue;
    // spdlog::trace("Updating V:");
    // spdlog::trace("-> Old Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
    particle.setV(Physics::calculateV(particle, delta_t));
    // spdlog::trace("-> New Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
  }
}

void CutoffSimulation::moveParticles() {
  for (int i = 0; i < linkedCells.cells.size(); i++) {
    Cell &current_cell = linkedCells.cells[i];

    for (int j = 0; j < current_cell.particles.size(); j++) {
      auto p = current_cell.particles[j];
      if (p->getType() < 0) continue;
      const int k = linkedCells.coordinate3dToIndex1d(p->getX());

      if (i == k) continue;

      // move p to cell[j]

      spdlog::trace("Moving particle with coordinate ({},{},{}) from cell {} to cell {}", p->getX()[0], p->getX()[1],
                    p->getX()[2], i, k);
      std::array<int, 3> i3D = linkedCells.index1dToIndex3d(i);
      spdlog::trace("Old cell: ({},{},{})", i3D[0], i3D[1], i3D[2]);
      Cell &new_cell = linkedCells.cells[k];
      if (new_cell.cell_type != CellType::GHOST) {
        new_cell.particles.push_back(p);
      } else {
        // get shared border current_cell, new_cell
        BorderType border = linkedCells.getSharedBorderType(i, k);

        if (border == OUTFLOW) {
          p->setType(-1);  // mark particle as dead
          spdlog::trace("Particle ({},{},{}) is dead!", p->getX()[0], p->getX()[1], p->getX()[2]);
        } else if (border == NAIVE_REFLECTION) { //Muss wahrscheinlich in Kombination mit Periodic noch ein update bekommen
          // First go back to the Old Position and then reflect the Velocity and calculate the new Position
          // This is not acurate, because the particle is not reflected at the border,
          // but since simply turning the velocity is not acurate either, this should be a good enough solution
          int borderIndex = linkedCells.getSharedBorder(i, k);
          Vector3 v = p->getV();
          v[borderIndex % 3] *= -1;
          Vector3 neg = {-1, -1, -1};
          p->setV(neg * p->getV());          // Turn Velocity
          Vector3 oldF = p->getOldF();       // Save OldF
          p->setF(neg * p->getF());          // Turn F
          Physics::calculateX(*p, delta_t);  // Calculate old Position
          p->setF(oldF);
          p->setF(neg * p->getF());          // Reset old Force
          p->setV(v);                        // Set new Velocity
          Physics::calculateX(*p, delta_t);  // Calculate new Position
          continue;                          // Don't move the Particle into a Ghost Cell
        } else if (border == PERIODIC) {
          Vector3 x = p->getX();
          spdlog::trace("Particle with position ({},{},{}) left domain at one side and entered it at the other side",
                        x[0], x[1], x[2]);
          for (int index = 0; index < 3; index++) {
            if (x[index] < 0) x[index] += linkedCells.domain_size[index];
            if (x[index] > linkedCells.domain_size[index]) x[index] -= linkedCells.domain_size[index];
          }
          p->setX(x);
          std::array<int, 3> newCellIndex3d = linkedCells.coordinate3dToIndex3d(x[0], x[1], x[2]);
          std::array<int, 3> oldCellIndex3d = linkedCells.index1dToIndex3d(i);
          std::array<int, 3> ghostCellIndex3d = linkedCells.index1dToIndex3d(k);
          int newCellIndex1d = linkedCells.index3dToIndex1d(newCellIndex3d[0], newCellIndex3d[1], newCellIndex3d[2]);
          linkedCells.cells[newCellIndex1d].particles.push_back(p);

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
}

void CutoffSimulation::updateGhost() {
  for (int cell_index = 0; cell_index < linkedCells.cells.size(); cell_index++) {
    auto &cell = linkedCells.cells[cell_index];
    if (cell.cell_type == CellType::GHOST) {
      // clear particles of ghost cells
      cell.size_ghost_particles = 0;
      continue;
    }
  }
  for (int cell_index = 0; cell_index < linkedCells.cells.size(); cell_index++) {
    auto &cell = linkedCells.cells[cell_index];
    if (cell.cell_type != BORDER) continue;
    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }
  }
}

void CutoffSimulation::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6; l++) {
    if (is2D && (l == 2 || l == 5)) continue;
    if (cell.borders[l] != REFLECTION) continue;

    // create ghost particles for every reflection border

    // X of ghost Particle
    Vector3 ghostParticleX = particle.getX();
    spdlog::trace("ghostParticleX: ({},{},{})", ghostParticleX[0], ghostParticleX[1], ghostParticleX[2]);
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
    spdlog::trace(
        "Adding Ghost Particle with coordinates ({}, {}, {}) (Ghost Particle of ({},{},{}))to Cell with index {}/{}",
        ghostParticleX[0], ghostParticleX[1], ghostParticleX[2], particle.getX()[0], particle.getX()[1],
        particle.getX()[2], ghostCellIndex1d, linkedCells.cells.size());
    Cell &ghost_cell = linkedCells.cells[ghostCellIndex1d];
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

void CutoffSimulation::initializeBrownianMotion() {
  for (auto &p : linkedCells.particles) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, (is2D ? 2 : 3)));
  }
}
