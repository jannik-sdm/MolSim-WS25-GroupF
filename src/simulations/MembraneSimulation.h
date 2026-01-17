//
// Created by jv_fedora on 12.01.26.
//

#pragma once
#include "ThermostatSimulation.h"

class MembraneSimulation : public ThermostatSimulation {
 private:
  double sqrt2 = std::sqrt(2.0);
  double r0;
  double stiffnessConstant;
  double F_zUp;
  std::vector<Particle *> upwardsParticles;

 public:
  MembraneSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                     const double delta_t, const std::optional<double> brown_motion_avg_velocity,
                     const Vector3 &dimension, const double cutoff_radius, const std::array<BorderType, 6> &border,
                     const bool is2D, const double g_grav, const std::optional<double> t_initial,
                     Thermostat &thermostat, double r0, double stiffnessConstant, double F_zUp,
                     std::vector<Particle *> &upwardsParticles)
      // Es ist ein bisschen Kriminell hier einfach den Cutoff Radius Manuell anzugeben, aber für den Anfang reicht es
      : ThermostatSimulation(particles, start_time, end_time, delta_t, brown_motion_avg_velocity, dimension,
                             std::pow(2, 1.0 / 6.0) * particles[0].getSigma(), border, is2D, g_grav, t_initial,
                             thermostat),
        r0(r0),
        stiffnessConstant(stiffnessConstant),
        F_zUp(F_zUp),
        upwardsParticles(upwardsParticles) {}
  virtual ~MembraneSimulation() override = default;

  void updateF() override;
};
