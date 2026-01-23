//
// Created by jannik on 12/14/25.
//
#pragma once
#include <cmath>
#include <vector>

#include "Particle.h"
#include "simulations/Thermostat.h"

/**
 * @class Thermostat
 * Thermostat used in Assignment 4
 *
 * Allows for the simulation to modify the temperature of the system
 */
class NanoScaleThermostat : public Thermostat {
 public:
  NanoScaleThermostat(std::vector<Particle> &particles, bool is2D, int n, double target_temperature,
                      double maximum_temperature_change)
      : Thermostat(particles, is2D, n, target_temperature, maximum_temperature_change) {};

  virtual ~NanoScaleThermostat() = default;

 private:
  /**
   * @brief Calculates the kinetic energy of the simulation (sum of energy of all particles)
   * @return Sum of the kinetic energy of all the particles
   */
  double calculateEkin();
};
