//
// Created by jannik on 11/6/25.
//

#pragma once

#include "../particleContainers/ParticleContainerV2.h"
#include "Simulation.h"

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
 public:
  PlanetSimulation(ParticleContainerV2 &container, double end_time, double delta_t);
  /**
   * Calculates one timestep of the simulation and applies the changes to the particles.
   */
  void iteration() override;

 protected:
  /** @brief Container for the particles */
  ParticleContainerV2 &container;
  /** @todo remove this variable, its unused */
  const double end_time;
  /** @brief timestep used in calculations */
  const double delta_t;

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
