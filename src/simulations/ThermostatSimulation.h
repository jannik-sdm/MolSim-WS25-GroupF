#pragma once

#include "Particle.h"
#include "container/linkedCells/Cell.h"
#include "simulations/CutoffSimulation.h"
#include "simulations/Thermostat.h"

class ThermostatSimulation : CutoffSimulation {
 protected:
  Thermostat &thermostat;

 public:
  ThermostatSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                       const double delta_t, const Vector3 &dimension, const double cutoff_radius,
                       const std::array<BorderType, 6> &border, const bool is2D, Thermostat &thermostat)
      : CutoffSimulation(particles, start_time, end_time, delta_t, dimension, cutoff_radius, border, is2D),
        thermostat(thermostat) {}

  void updateV() override;
};
