//
// Created by jannik on 11/7/25.
//

#pragma once

#include "Particle.h"
#include "simulations/PlanetSimulation.h"

/**
 * @class CollisionSimulation
 * @brief Simulation for Assignment 2
 *
 * This class calculates timesteps for a particle collision simulation.
 *
 * @see Physics::calculateV
 * @see Physics::calculateX
 * @see Physics::lennardJonesForce
 */
class CollisionSimulation : public PlanetSimulation {
 public:
  /**
   * @brief Constructs a CollisionSimulation
   * @param particles reference to the particles array
   * @param start_time start time of the simulation
   * @param end_time end time of the simulation
   * @param delta_t timestep of the simulation
   * @param brown_motion_avg_velocity avg velocity to initalize the brownian motion
   */
  CollisionSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                      const double delta_t, const std::optional<double> brown_motion_avg_velocity)
      : PlanetSimulation(particles, start_time, end_time, delta_t) {
    if (brown_motion_avg_velocity.has_value()) {
      initializeBrownianMotion(brown_motion_avg_velocity.value());
    }
  }
  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the Lennard-Jones force f.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  void updateF() override;

  /**
   * @brief Add brownian motion
   *
   * Adds a brownian motion in the form of velocity to all particles
   * @param brown_motion_avg_velocity
   */
  void initializeBrownianMotion(double brown_motion_avg_velocity);
};
