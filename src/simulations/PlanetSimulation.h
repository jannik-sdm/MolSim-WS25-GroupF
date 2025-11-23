//
// Created by jannik on 11/6/25.
//

#pragma once

#include "../ParticleContainer.h"
#include "Simulation.h"

class PlanetSimulation : public Simulation {
 public:
  PlanetSimulation(ParticleContainer &container, double end_time, double delta_t);
  /**
   * Calculates one timestep of the simulation and applies the changes to the particles.
   */
  void iteration() override;

 protected:
  ParticleContainer &particleContainer;
  const double end_time;
  const double delta_t;

  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the force between the two particles.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  virtual void calculateF();

  /**
   * @brief calculate the position for all particles
   *
   * For each particle i this function calculates the new position x.
   */
  virtual void calculateX();

  /**
   * @brief calculate the Velocity for all particles
   *
   * For each particle i this function calculates the new Velocity v
   */
  virtual void calculateV();
};
