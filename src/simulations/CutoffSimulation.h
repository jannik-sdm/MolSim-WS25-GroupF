//
// Created by jannik on 11/23/25.
//
#pragma once

#include <cmath>
#include <memory>

#include "container/linkedCells/LinkedCells.h"
#include "simulations/Simulation.h"
#include "simulations/Thermostat.h"

class CutoffSimulation : public Simulation {
 protected:
  bool is2D;
  const double g_grav;
  LinkedCells linkedCells;
  std::vector<Particle> &particles;

  /**
   * A counter for the amount of particles that are still alive
   */

 public:
  // TODO: bisschen scuffed mit der repulsing distance, weiß nicht ob das funktioniert aber versuche es mal so und
  // später vlt fixen
  CutoffSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t, const std::optional<double> brown_motion_avg_velocity, const Vector3 &dimension, const double cutoff_radius,
                   const std::array<BorderType, 6> &border, const bool is2D, double g_grav)
      : Simulation(start_time, end_time, delta_t),
        is2D(is2D),
        g_grav(g_grav),
        linkedCells(particles, dimension, cutoff_radius, is2D, border),
        particles(particles) {
    if (brown_motion_avg_velocity.has_value()) {
      initializeBrownianMotion(brown_motion_avg_velocity.value());
    }
  }

  /**
   * Performes one iteration of the simulation by updating the force, position and velocity of each particle
   */
  void iteration() override;

  /**
   * Calculates the forces between each particle up to a specified cutoff radius and updates them
   */
  void updateF() override;
  /**
   * Updates the position of each particle and moves the pointers from cell to cell
   */
  void updateX() override;
  /**
   * Updates the velocity of each particle
   */
  void updateV() override;

  /**
   * @brief getter for the tests
   */
  LinkedCells &getLinkedCells() { return linkedCells; }

  /**
   * Initializes the brownian motion
   */
  void initializeBrownianMotion(double brown_motion_avg_velocity);
};
