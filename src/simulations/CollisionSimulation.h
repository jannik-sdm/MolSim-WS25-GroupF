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
  CollisionSimulation(std::vector<Particle> &particles, double end_time, double delta_t);
  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the Lennard-Jones force f.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  void updateF() override;

 private:
  /** @brief Value used for epsilon durign force calculation, @see Physics::lennardJonesForce */
  const double epsilon = 5;
  /** @brief Value used for sigma during force calculation, @see Physics::lennardJonesForce */
  const double sigma = 1;
  /** @brief Average velocity used to initialize brownian motion */
  const double brownian_motion_avg_velocity = 0.1;
};
