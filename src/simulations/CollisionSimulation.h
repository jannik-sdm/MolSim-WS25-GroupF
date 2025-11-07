//
// Created by jannik on 11/7/25.
//

#pragma once
#include "PlanetSimulation.h"
#include "../Particle.h"

class CollisionSimulation : public PlanetSimulation {
  public:

  CollisionSimulation(ParticleContainer &container, double end_time, double delta_t);
  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the force f with the Formula: \f[ F_{ij} =
   * - \frac{24 \cdot \epsilon}{(\|x_i - x_j\|_2)^2} \left( \left( \frac{\sigma}{\|x_i - x_j\|_2} \right)^6 - 2 \left(
   * \frac{\sigma}{\|x_i - x_j\|_2} \right)^{12} \right) (x_i - x_j)\f]. Then this function sums up all forces for one
   * particle to calculate the effective force of each particle
   */
  void calculateF() override;
private:
  // constants for the cacluation
  const double epsilon = 5;
  const double sigma = 1;
  const double brownian_motion_avg_velocity = 0.1;
};

