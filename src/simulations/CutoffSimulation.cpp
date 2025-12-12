//
// Created by jannik on 11/23/25.
//
#include "simulations/CutoffSimulation.h"

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "Physics.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

CutoffSimulation::CutoffSimulation(std::vector<Particle> &particles, Vector3 dimension, double end_time, double delta_t,
                                   double cutoffRadius, std::array<BorderType, 6> &border, bool is2D)
    : end_time(end_time),
      delta_t(delta_t),
      cutoffRadius(cutoffRadius),
      linkedCells(particles, dimension, cutoffRadius, border),
      particles(particles),
      repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma),
      is2D(is2D) {
  for (auto &p : particles)
    if (p.getState() != -1) alive_particles++;
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

        if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

        // spdlog::warn("{} <-> {}", p1->toString(), p2->toString());
        Vector3 f = Physics::LennardJones::force(*p1, *p2, sigma, epsilon);
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
    NeighBourIndices neighbourCellsIndex = linkedCells.getNeighbourCells(i);

    for (const int j : neighbourCellsIndex) {
      auto &c2 = linkedCells.cells[j];
      // newton optimization, but ONLY if c2 is not a Ghost cell, because if this calculation is skipped, particles are
      // not repulsed
      if (j < i && c2.cell_type != CellType::GHOST) continue;

      for (const auto p1 : c1.particles) {
        // iterate over ghost particles if c2 is a ghost cell, else use normale particles
        if (c2.cell_type == CellType::GHOST) {
          for (int k = 0; k < c2.size_ghost_particles; k++) {
            Particle &p2 = c2.ghost_particles[k];
            const double distance = ArrayUtils::L2Norm(p1->getX() - p2.getX());
            // for ghost particles the force should only be computed if its repulsing
            // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
            spdlog::trace("reached radius check for ghost particles");
            if (distance >= this->repulsing_distance || distance > cutoffRadius) continue;

            Vector3 f = Physics::LennardJones::force(*p1, p2, sigma, epsilon);
            p1->addF(f);
            spdlog::trace("adding force of ghost particle: {} {} {}", f[0], f[1], f[2]);
            // dont need to subtract force of ghost particles, since they are updated after anyways
          }
        } else {
          // case for regular cells
          for (const auto p2 : c2.particles) {
            if (ArrayUtils::L2Norm(p1->getX() - p2->getX()) > cutoffRadius) continue;

            Vector3 f = Physics::LennardJones::force(*p1, *p2, sigma, epsilon);
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
    if (particle.getState() < 0) continue;
    spdlog::trace("Updating X:");
    spdlog::trace("-> Old Position: ({},{},{})", particle.getX()[0], particle.getX()[1], particle.getX()[2]);
    particle.setX(Physics::StoermerVerlet::position(particle, delta_t));
    spdlog::trace("-> New: ({},{},{})", particle.getX()[0], particle.getX()[1], particle.getX()[2]);
  }
}

void CutoffSimulation::updateV() {
  for (auto &particle : linkedCells.particles) {
    if (particle.getState() < 0) continue;
    spdlog::trace("Updating V:");
    spdlog::trace("-> Old Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
    particle.setV(Physics::StoermerVerlet::velocity(particle, delta_t));
    spdlog::trace("-> New Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
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

      spdlog::trace("Moving particle with coordinate ({},{},{}) from cell {} to cell {}", p->getX()[0], p->getX()[1],
                    p->getX()[2], k, i);
      Cell &new_cell = linkedCells.cells[k];
      if (new_cell.cell_type != CellType::GHOST) {
        new_cell.particles.push_back(p);
      } else {
        // get shared border current_cell, new_cell
        int borderIndex = linkedCells.getSharedBorder(i, k);
        BorderType border = current_cell.borders[borderIndex];
        if (border == BorderType::OUTFLOW) {
          p->setState(-1);  // mark particle as dead
          alive_particles--;
          spdlog::trace("Particle ({},{},{}) is dead!", p->getX()[0], p->getX()[1], p->getX()[2]);
        } else if (border == BorderType::NAIVE_REFLECTION) {
          // First go back to the Old Position and then reflect the Velocity and calculate the new Position
          // This is not acurate, because the particle is not reflected AT the border,
          // but since simply turning the velocity is not acurate either, this should be a good enough solution
          Vector3 v = p->getV();
          v[borderIndex % 3] *= -1;
          Vector3 neg = {-1, -1, -1};
          p->setV(neg * p->getV());                        // Turn Velocity
          Vector3 oldF = p->getOldF();                     // Save OldF
          p->setF(neg * p->getF());                        // Turn F
          Physics::StoermerVerlet::position(*p, delta_t);  // Calculate old Position
          p->setF(oldF);
          p->setF(neg * p->getF());                        // Reset old Force
          p->setV(v);                                      // Set new Velocity
          Physics::StoermerVerlet::position(*p, delta_t);  // Calculate new Position
          continue;                                        // Don't move the Particle into a Ghost Cell
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
    if (cell.cell_type != CellType::BORDER) continue;
    for (auto particle : cell.particles) {
      // iterate over all particles of the BORDER cell and create ghost particles for each of them
      createGhostParticles(*particle, cell_index, cell);
    }
  }
}

void CutoffSimulation::createGhostParticles(Particle &particle, const int cell_index, Cell &cell) {
  for (int l = 0; l < 6; l++) {
    if (is2D && (l == 2 || l == 5)) continue;
    if (cell.borders[l] != BorderType::REFLECTION) continue;

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

double CutoffSimulation::calculateEkin() {
  double ekin = 0;
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    const double v = ArrayUtils::L2Norm(p.getV());
    ekin += 0.5 * p.getM() * v * v;
  }
  return ekin;
}

double CutoffSimulation::calculateCurrentTemperature() {
  const int dimensions = (is2D ? 2 : 3);
  const double temperature = (2 * calculateEkin()) / (dimensions * alive_particles);
  return temperature;
}

double CutoffSimulation::calculateScalingFactor(double target_temperature) {
  return std::sqrt(target_temperature / calculateCurrentTemperature());
}

void CutoffSimulation::applyScalingFactor(double scaling_factor) {
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    p.setV(scaling_factor * p.getV());
  }
}