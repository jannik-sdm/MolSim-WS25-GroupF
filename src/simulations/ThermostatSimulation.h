#pragma once

#include "Particle.h"
#include "container/linkedCells/Cell.h"
#include "simulations/CutoffSimulation.h"
#include "simulations/Thermostat.h"

class ThermostatSimulation : public CutoffSimulation {
 protected:
  /**
   * Thermostat that implements several methods to control the temperature of the system
   */
  Thermostat &thermostat;

 public:
  ThermostatSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                       const double delta_t, const std::optional<double> brown_motion_avg_velocity, const Vector3 &dimension, const double cutoff_radius,
                       const std::array<BorderType, 6> &border, const bool is2D, const double g_grav, const std::optional<double> t_initial,
                       Thermostat &thermostat)
      : CutoffSimulation(particles, start_time, end_time, delta_t, brown_motion_avg_velocity,dimension, cutoff_radius, border, is2D, g_grav),
        thermostat(thermostat) {
    if (t_initial.has_value()) {
      initializeBrownianMotionWithTemperature(t_initial.value());
    }
  }
  virtual ~ThermostatSimulation() = default;
  /**
   * Updates the velocity according to Stoermer Verlet and applies the thermostat if needed to control the temperature
   * of the system
   */
  void updateV() override;

  void initializeBrownianMotionWithTemperature(const double init_temperature);
};
