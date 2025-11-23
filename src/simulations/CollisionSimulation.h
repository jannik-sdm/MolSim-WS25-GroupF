//
// Created by jannik on 11/7/25.
//

#pragma once

#include "PlanetSimulation.h"

class CollisionSimulation : public PlanetSimulation {
 public:
  CollisionSimulation(ParticleContainer &container, double end_time, double delta_t);
  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the Lennard-Jones force f.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  void updateF() override;

 private:
  // constants for the cacluation
  const double epsilon = 5;
  const double sigma = 1;
  double brownian_motion_avg_velocity = 0.1;
};
