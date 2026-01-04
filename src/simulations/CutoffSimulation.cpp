//
// Created by jannik on 11/23/25.
//
#include "simulations/CutoffSimulation.h"

#include <spdlog/spdlog.h>
#include <unistd.h>

#include "Physics.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void CutoffSimulation::iteration() {
  spdlog::debug("Updating Positions");
  updateX();
  spdlog::debug("Updating Forces");
  updateF();
  spdlog::debug("Updating Velocities");
  updateV();
}

void CutoffSimulation::updateF() {
  // set the force of all particles to zero
  linkedCells.applyToParticles([this](Particle &p) { p.setF({0, g_grav * p.getM(), 0}); });

  linkedCells.applyToPairs([this](Particle &p1, Particle &p2) {
    Vector3 f = Physics::LennardJones::force(p1, p2, sigma, epsilon);
    p1.addF(f);
    p2.subF(f);
  });
}
/*for (int i = 0; i < linkedCells.cells.size(); i++) {
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
            // Echte Zelle zu Ghost Zelle finden -> Funktioniert auch über mehrere Dimensionen
            int realCellIndex = linkedCells.getPeriodicEquivalentForGhost(j);
            auto &realCell = linkedCells.cells[realCellIndex];
            // N3
            if (realCellIndex < i) continue;
            for (const auto p2 : realCell.particles) {
              if (p1 == p2) continue;  // Nur zur Sicherheit

              // Calculate new Position relative to ghost cell
              const std::array<int, 3> ghost = linkedCells.index1dToIndex3d(j);
              const std::array<int, 3> real = linkedCells.index1dToIndex3d(realCellIndex);
              Vector3 delta = {0.0};
              for (int _i = 0; _i < 2; _i++)
                delta[_i] = static_cast<double>(ghost[_i] - real[_i]) * linkedCells.cell_size[_i];
              Vector3 new_pos = p2->getX() + delta;

              double distance = ArrayUtils::L2Norm(p1->getX() - new_pos);


              if (distance <= cutoffRadius && distance > 0) {
                Particle np = Particle(*p2);
                np.setX(new_pos);

                Vector3 f = Physics::lennardJonesForce(*p1, np, sigma, epsilon);
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
              if (distance >= this->repulsing_distance || distance > cutoffRadius) continue;*/

void CutoffSimulation::updateX() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getState() < 0) return;
    spdlog::trace("Updating X:");
    spdlog::trace("-> Old Position: ({},{},{})", p.getX()[0], p.getX()[1], p.getX()[2]);
    p.setX(Physics::StoermerVerlet::position(p, delta_t));
    spdlog::trace("-> New: ({},{},{})", p.getX()[0], p.getX()[1], p.getX()[2]);
  });
  linkedCells.moveParticles();
}

void CutoffSimulation::updateV() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getState() < 0) return;
    spdlog::trace("Updating V:");
    spdlog::trace("-> Old Velocity: ({},{},{})", p.getV()[0], p.getV()[1], p.getV()[2]);
    p.setV(Physics::StoermerVerlet::velocity(p, delta_t));
    spdlog::trace("-> New Velocity: ({},{},{})", p.getV()[0], p.getV()[1], p.getV()[2]);
  });
}

void CutoffSimulation::initializeBrownianMotion() {
  linkedCells.applyToParticles([this](Particle &p) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, (is2D ? 2 : 3)));
  });
}
              /*void CutoffSimulation::moveParticles() {
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
}*/
