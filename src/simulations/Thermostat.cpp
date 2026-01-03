//
// Created by jannik on 12/14/25.
//
#include "Thermostat.h"

#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

Thermostat::Thermostat(std::vector<Particle> &particles, bool is2D, int n, double target_temperature,
                       double maximum_temperature_change, double initial_temperature_optional,
                       double average_brownian_velocity_optional)
    : particles(particles),
      n(n),
      target_temperature(target_temperature),
      maximum_temperature_change(maximum_temperature_change),
      is2D(is2D) {
  if (initial_temperature_optional >= 0) {
    if (initial_temperature_optional >= 0.0) {
      initializeBrownianMotion(initial_temperature_optional);
    }
  } else if (average_brownian_velocity_optional >= 0) {
    initializeBrownianMotion(average_brownian_velocity_optional);
  }
  current_temperature = calculateCurrentTemperature(calculateAliveParticles());
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

double Thermostat::calculateCurrentTemperature(int alive_particles) {
  if (alive_particles == 0) return 0.0;

  const int dimensions = (is2D ? 2 : 3);
  const double temperature = (2 * calculateEkin()) / (dimensions * alive_particles);
  return temperature;
}

double Thermostat::calculateScalingFactor(int alive_particles) {
  return std::sqrt(target_temperature / calculateCurrentTemperature(alive_particles));
}

double Thermostat::calculateMaximumScalingFactor() {
  const int sign = current_temperature < target_temperature ? 1 : -1;
  return std::sqrt((current_temperature + sign * maximum_temperature_change) / current_temperature);
}

void Thermostat::updateTemperature(int alive_particles) {
  double scaling_factor = calculateScalingFactor(alive_particles);
  // new temperature can be calculated without taking into account v' by applying the squared scaling factor to the
  // current temperature
  const double new_temperature = scaling_factor * scaling_factor * current_temperature;
  const double delta = fabs(current_temperature - new_temperature);
  if (delta > maximum_temperature_change) {
    // scaling factor is too large for the system to handle ==> need to calculate maximum scaling factor
    // TODO: maybe optimize by storing the maximum scaling factor, but have to be careful if we overshoot the targeted
    // TODO: temperature and the sign of delta_t has to change
    scaling_factor = calculateMaximumScalingFactor();
  }
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    p.setV(scaling_factor * p.getV());
  }
}

int Thermostat::calculateAliveParticles() {
  int count = 0;
  for (auto &p : particles) {
    if (p.getState() != -1) {
      count++;
    }
  }

  return count;
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