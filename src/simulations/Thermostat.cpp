//
// Created by jannik on 12/14/25.
//

#include "Thermostat.h"

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

Thermostat::Thermostat(std::vector<Particle> &particles, int n, double target_temperature,
                       double maximum_temperature_change, int &alive_particles, bool is2D,
                       double initial_temperature_optional, double average_brownian_velocity_optional)
    : particles(particles),
      n(n),
      target_temperature(target_temperature),
      maximum_temperature_change(maximum_temperature_change),
      alive_particles(alive_particles),
      is2D(is2D) {
  if (initial_temperature_optional >= 0) {
    if (initial_temperature_optional >= 0.0) {
      initializeBrownianMotion(initial_temperature_optional);
    }
  } else if (average_brownian_velocity_optional >= 0) {
    initializeBrownianMotion(average_brownian_velocity_optional);
  }
  current_temperature = calculateCurrentTemperature();
}
double Thermostat::calculateEkin() {
  double ekin = 0;
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    const double v = ArrayUtils::L2Norm(p.getV());
    ekin += p.getM() * v * v;
  }
  return 0.5 * ekin;
}

double Thermostat::calculateCurrentTemperature() {
  if (alive_particles == 0) return 0.0;

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

void Thermostat::initializeBrownianMotionZero(double initial_temperature) {
  for (auto &p : particles) {
    const double factor = sqrt(initial_temperature / p.getM());
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(factor, (is2D ? 2 : 3)));
  }
}

void Thermostat::initializeBrownianMotion(double brownian_motion_avg_velocity) {
  for (auto &p : particles) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(brownian_motion_avg_velocity, (is2D ? 2 : 3)));
  }
}