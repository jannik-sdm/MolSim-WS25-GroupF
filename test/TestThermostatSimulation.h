#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Particle.h"
#include "simulations/CutoffSimulation.h"
#include "simulations/ThermostatSimulation.h"

class TestThermostatSimulation : public ::testing::Test {
 public:
  std::vector<Particle> particles;
  std::unique_ptr<ThermostatSimulation> sim;
  std::unique_ptr<Thermostat> thermostat;

  Vector3 domain = {10.0, 10.0, 10.0};
  double start_time = 0;
  double end_time = 10.0;
  double delta_t = 0.001;
  double cutoff = 2.5;
  bool is2D = false;
  double gravity = 0;
  std::array<BorderType, 6> borders = {BorderType::REFLECTION, BorderType::REFLECTION, BorderType::REFLECTION,
                                       BorderType::REFLECTION, BorderType::REFLECTION, BorderType::REFLECTION};

  double target_temp = 20.0;
  double max_temp_change = 1000.0;
  int thermostat_interval = 10;

  void SetUp() override {
    particles.clear();
    particles.reserve(100);
  }

  void InitSimulation() {
    thermostat = std::make_unique<Thermostat>(particles, is2D, thermostat_interval, target_temp, max_temp_change);

    sim = std::make_unique<ThermostatSimulation>(particles, start_time, end_time, delta_t, domain, cutoff, borders,
                                                 is2D, gravity, *thermostat);
  }

  // Helper to calculate velocity needed for a specific temperature
  // Formula: T = (m * v^2) / (dimensions * 1_particle)
  // Therefore: v = sqrt( (T * dimensions) / m )
  Vector3 getVelocityForTemp(double T, double mass = 1.0) {
    double dimensions = is2D ? 2.0 : 3.0;
    double v_scalar = std::sqrt((T * dimensions) / mass);
    return {v_scalar, 0, 0};
  }
};