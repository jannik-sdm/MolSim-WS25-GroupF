//
// Created by jannik on 12/14/25.
//

#include "Thermostat.h"

#include "utils/ArrayUtils.h"

Thermostat::Thermostat(std::vector<Particle> &particles, int n, double target_temperature,
                       double maximum_temperature_change, int &alive_particles, bool is2D)
    : particles(particles),
      n(n),
      target_temperature(target_temperature),
      maximum_temperature_change(maximum_temperature_change),
      alive_particles(alive_particles),
      is2D(is2D) {
  initial_temperature = calculateCurrentTemperature();
  if (initial_temperature == 0) {
  }
}

double Thermostat::calculateEkin() {
  double ekin = 0;
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    const double v = ArrayUtils::L2Norm(p.getV());
    ekin += 0.5 * p.getM() * v * v;
  }
  return ekin;
}

double Thermostat::calculateCurrentTemperature() {
  const int dimensions = (is2D ? 2 : 3);
  const double temperature = (2 * calculateEkin()) / (dimensions * alive_particles);
  return temperature;
}

double Thermostat::calculateScalingFactor() { return std::sqrt(target_temperature / calculateCurrentTemperature()); }

void Thermostat::applyScalingFactor(double scaling_factor) {
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    p.setV(scaling_factor * p.getV());
  }
}