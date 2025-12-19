//
// Created by jannik on 11/6/25.
//

#pragma once

#include "container/directSum/DirectSum.h"
#include "simulations/Simulation.h"

/**
 * @class PlanetSimulation
 * @brief Simulation for Assignment 1
 *
 * This class calculates timesteps for a planet simulation.
 *
 * @see Physics::calculateV
 * @see Physics::calculateX
 * @see Physics::planetForce
 */
class PlanetSimulation : public Simulation {
 protected:
  /** @brief Container for the particles */
  DirectSum container;

 public:
  PlanetSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t)
      : Simulation(start_time, end_time, delta_t), container(particles) {}
  /**
   * Calculates one timestep of the simulation and applies the changes to the particles.
   */
  void iteration() override;

  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the force between the two particles.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  virtual void updateF() override;

  /**
   * @brief calculate the position for all particles
   *
   * For each particle i this function calculates the new position x.
   */
  virtual void updateX() override;

  /**
   * @brief calculate the Velocity for all particles
   *
   * For each particle i this function calculates the new Velocity v
   */
  virtual void updateV() override;
};
