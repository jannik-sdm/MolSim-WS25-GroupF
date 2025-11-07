//
// Created by jannik on 11/6/25.
//

#pragma once
#include "ParticleContainer.h"
#include "Simulation.h"

class PlanetSimulation : public Simulation {
public:

  PlanetSimulation(ParticleContainer &container, double end_time, double delta_t);
  /**
    * Calculates one timestep of the simulation and applies the changes to the particles.
    */
  void iteration() override;

private:
  ParticleContainer &particleContainer;
  const double end_time;
  const double delta_t;

  /**
 * @brief calculate the force for all particles
 *
 * For each pair of disjunct particles this function calculates the force f with the Formula: \f$ F_{ij} =
 * \frac{m_im_j}{(||x_i-x_j||_2)^3}(x_j-x_i)\f$.
 * Then this function sums up all forces for one particle to calculate the effective force of each particle
 */
  void calculateF();

  /**
   * @brief calculate the position for all particles
   *
   * For each particle i this function calculates the position x: \f$ x_i(t_{n+1}) = x_i(t_n)+\Delta t \cdot v_i(t_n) +
   * (\Delta t)^2 \frac{F_i(t_n)}{2m_i}\f$
   */
  void calculateX();

  /**
   * @brief calculate the Velocity for all particles
   *
   * For each particle i this function calculates the Velocity v: \f$ v_i(t_{n+1}) = v_i(t_n)+\Delta t
   * \frac{F_i(t_n)+F_i(t_{n+1})}{2m_i}\f$
   */
  void calculateV();
};

