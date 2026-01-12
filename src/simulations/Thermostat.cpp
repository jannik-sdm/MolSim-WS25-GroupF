//
// Created by jannik on 12/14/25.
//
#include "Thermostat.h"

#include "ThermostatSimulation.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

Thermostat::Thermostat(LinkedCells &linked_cells, bool is2D, int n, double target_temperature,
                       double maximum_temperature_change)
    : linked_cells(linked_cells),
      n(n),
      target_temperature(target_temperature),
      maximum_temperature_change(maximum_temperature_change),
      is2D(is2D) {
  current_temperature = calculateCurrentTemperature();
}

double Thermostat::calculateEkin() {
  double ekin = 0;
  linked_cells.applyToParticles([&](Particle &p) {
    if (p.getState() < 0) return;

    // Optimization: Avoid Sqrt()
    const Vector3 &v = p.getV();
    double v2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    ekin += p.getM() * v2;
  });

  return 0.5 * ekin;
}

double Thermostat::calculateCurrentTemperature() {
  if (linked_cells.alive_particles == 0) return 0.0;

  const int dimensions = (is2D ? 2 : 3);
  const double temperature = (2 * calculateEkin()) / (dimensions * linked_cells.alive_particles);
  return temperature;
}

double Thermostat::calculateScalingFactor() {
  if (current_temperature < 1e-10) {
    // safety check if temperature is 0 and not initialized with brownian motion, to prevent division by 0
    return 1.0;
  }
  return std::sqrt(target_temperature / current_temperature);
}

void Thermostat::updateTemperature() {
  current_temperature = calculateCurrentTemperature();
  spdlog::info("temperature before thermostat", current_temperature);
  if (current_temperature < 1e-10) {
    return;  // Can't scale 0 temperature
  }

  double delta_t = target_temperature - current_temperature;
  // use clamp to return the highest temperature change
  delta_t = std::clamp(delta_t, -maximum_temperature_change, maximum_temperature_change);
  spdlog::info("delta_t {}", delta_t);
  double new_temperature = current_temperature + delta_t;

  // calculate scaling factor with new temperature
  double scaling_factor = std::sqrt(new_temperature / current_temperature);

  linked_cells.applyToParticles([&](Particle &p) {
    if (p.getState() < 0) return;
    p.setV(scaling_factor * p.getV());
  });
}

void Thermostat::initializeBrownianMotionZero(double initial_temperature) {
  linked_cells.applyToParticles([&](Particle &p) {
    const double factor = sqrt(initial_temperature / p.getM());
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(factor, is2D ? 2 : 3));
  });
}

void Thermostat::initializeBrownianMotion(double brownian_motion_avg_velocity) {
  linked_cells.applyToParticles([&](Particle &p) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, is2D ? 2 : 3));
  });
}