//
// Created by jannik on 12/14/25.
//
#pragma once
#include <cmath>
#include <vector>

#include "Particle.h"
#include "simulations/Thermostat.h"

/**
 * @class NanoScaleThermostat
 * Thermostat used in Assignment 5
 *
 * Allows for the simulation to modify the temperature of the system,
 * but only looks at particles with type less than NanoScaleSimulation::MAX_STATIC_TYPE
 */
class NanoScaleThermostat : public Thermostat {
 public:
  NanoScaleThermostat(std::vector<Particle> &particles, bool is2D, int n, double target_temperature,
                      double maximum_temperature_change)
      : Thermostat(particles, is2D, n, target_temperature, maximum_temperature_change) {};

  virtual ~NanoScaleThermostat() = default;

 private:
  /**
   * @copydoc Thermostat::calculateEkin()
   */
  double calculateEkin();
};
