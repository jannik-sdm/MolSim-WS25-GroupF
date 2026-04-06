#pragma once

#include "container/linkedCells/LinkedCells.h"

/**
 * @copydoc LinkedCells
 *
 * Contains an alternative parallelisation strategy based on taskloops
 */
class LinkedCellsV2 : public LinkedCells {
 public:
  using LinkedCells::LinkedCells;

  /**
   * @copydoc LinkedCells::applyToPairs()
   */
  template <typename Function>
  inline void applyToPairs(Function f) {
#pragma omp parallel
#pragma omp single
    {
      // Calculate forces in own cell
#pragma omp taskloop
      for (Cell &cell : cells) {
        for (int i = 0; i < cell.particles.size(); i++) {
          const auto p1 = cell.particles[i];

          for (int j = i + 1; j < cell.particles.size(); j++) {
            const auto p2 = cell.particles[j];

            const Vector3 diff = p1->getX() - p2->getX();
            const double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
            if (r2 > cutoffSquared) continue;

            f(*p1, *p2);
          }
        }
      }

      // Calculate forces with neighbour cells
#pragma omp taskloop
      for (const int i : innerCells) {
        auto &c1 = cells[i];
        NeighBourIndices neighbourCellsIndex = getNeighbourCells(i);

        for (const int j : neighbourCellsIndex) {
          auto &c2 = cells[j];
          if (j < i && c2.cell_type != CellType::BORDER) continue;
          for (const auto p1 : c1.particles) {
            for (const auto p2 : c2.particles) {
              const Vector3 diff = p1->getX() - p2->getX();
              const double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
              if (r2 > cutoffSquared) continue;
              f(*p1, *p2);
              // SPDLOG_INFO("F: {} {} {}", f[0], f[1], f[2]);
            }
          }
        }
      }
    }

#pragma omp taskloop
    for (const int i : borderCells) {
      auto &c1 = cells[i];
      NeighBourIndices neighbourCellsIndex = getNeighbourCells(i);
      for (const int j : neighbourCellsIndex) {
        auto &c2 = cells[j];
        if (j < i && c2.cell_type != CellType::GHOST) continue;
        if (c2.cell_type == CellType::GHOST) {
          auto borderType = getSharedBorderType(i, j);
          if (borderType == BorderType::PERIODIC) {
            for (const auto p1 : c1.particles) {
              for (int k = 0; k < c2.size_ghost_particles; k++) {
                Particle &p2 = c2.ghost_particles[k];
                const Vector3 diff = p1->getX() - p2.getX();
                const double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
                if (r2 > cutoffSquared) continue;
                f(*p1, p2);
              }
            }
          } else {
            for (const auto p1 : c1.particles) {
              for (int k = 0; k < c2.size_ghost_particles; k++) {
                Particle &p2 = c2.ghost_particles[k];
                const Vector3 diff = p1->getX() - p2.getX();
                const double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
                // for ghost particles the force should only be computed if its repulsing
                // normally cutoffRadius >> repulsing_distance but i'm letting it stand since it's an or statement
                SPDLOG_TRACE("reached radius check for ghost particles");
                const double repusling_distance = calcRepulsingDistance(p1->getSigma(), p2.getSigma());
                if (r2 >= repusling_distance * repusling_distance || r2 > cutoffSquared) continue;

                f(*p1, p2);
              }
            }
          }
        } else {
          // case for regular cells
          for (const auto p1 : c1.particles) {
            for (const auto p2 : c2.particles) {
              const Vector3 diff = p1->getX() - p2->getX();
              const double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];
              if (r2 > cutoffSquared) continue;
              f(*p1, *p2);
              // SPDLOG_INFO("F: {} {} {}", f[0], f[1], f[2]);
            }
          }
        }
      }
    }
  }
};
