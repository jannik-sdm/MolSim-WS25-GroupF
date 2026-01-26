#pragma once

#include <filesystem>

#include "Particle.h"
#include "container/linkedCells/Cell.h"
#include "simulations/ThermostatSimulation.h"
#include "simulations/nanoScale/NanoScaleThermostat.h"

/**
 * @class ThermostatSimulation
 * @brief Simulation for Assignment 4
 *
 * This class calculates timesteps for a particle simulation with a cutoff radius, different boundary types, different
 * particle types and a thermostat
 *
 * @see Physics::calculateV
 * @see Physics::calculateX
 * @see Physics::LennardJones::fastForce
 */
class NanoScaleSimulation : public ThermostatSimulation {
 public:
  /** Particles with a type less or equal to this will not be moved */
  const static int MAX_STATIC_TYPE = 1;
  const unsigned int BINS = 50;

 public:
  NanoScaleSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                      const double delta_t, const std::optional<double> brown_motion_avg_velocity,
                      const Vector3 &dimension, const double cutoff_radius, const std::array<BorderType, 6> &border,
                      const bool is2D, const double g_grav, const std::optional<double> t_initial,
                      Thermostat &thermostat)
      : ThermostatSimulation(particles, start_time, end_time, delta_t, brown_motion_avg_velocity, dimension,
                             cutoff_radius, border, is2D, g_grav, t_initial, thermostat) {
    // also sets types of the particles
    initializeParticleTypes();
    initializeMixingTable();
    if (t_initial.has_value()) {
      initializeBrownianMotionWithTemperature(t_initial.value());
    }
  }

  virtual ~NanoScaleSimulation() = default;

  /**
   * Updates the position according to Stoermer Verlet
   */
  void updateX() override;

  /**
   * Updates the velocity according to Stoermer Verlet and applies the thermostat if needed to control the temperature
   * of the system
   */
  void updateV() override;

  /**
   * Calculates the forces between each particle up to a specified cutoff radius and updates them
   */
  void updateF() override;

  /**
   * Initializes the system with the brownian motion, based on the given initial temperature
   * @param init_temperature The initial temperature of the system
   */
  void initializeBrownianMotionWithTemperature(const double init_temperature) override;

  void calculateStatistics(const std::filesystem::path &filename);
};
