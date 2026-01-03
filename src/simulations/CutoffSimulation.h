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
  const double epsilon = 5;
  const double sigma = 1;
  const double brownian_motion_avg_velocity = 0.1;
  bool is2D;
  /**
   * Distance for when two particles are repulsing
   */
  const double repulsing_distance = std::pow(2, 1.0 / 6.0) * sigma;
  LinkedCells linkedCells;
  std::vector<Particle> &particles;

  /**
   * A counter for the amount of particles that are still alive
   */

 public:
  // TODO: bisschen scuffed mit der repulsing distance, weiß nicht ob das funktioniert aber versuche es mal so und
  // später vlt fixen
  CutoffSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t, const Vector3 &dimension, const double cutoff_radius,
                   const std::array<BorderType, 6> &border, const bool is2D)
      : Simulation(start_time, end_time, delta_t),
        is2D(is2D),
        linkedCells(particles, dimension, cutoff_radius, is2D, repulsing_distance, border),
        particles(particles) {
    initializeBrownianMotion();
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
  void initializeBrownianMotion();
};
