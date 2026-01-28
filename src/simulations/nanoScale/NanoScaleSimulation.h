#pragma once

#include <filesystem>

#include "Particle.h"
#include "container/linkedCells/Cell.h"
#include "simulations/ThermostatSimulation.h"
#include "simulations/nanoScale/NanoScaleThermostat.h"

/**
 * @class NanoScaleSimulation
 * @brief Simulation for Assignment 5
 *
 * This class calculates timesteps for a particle simulation with a cutoff radius, different boundary types, different
 * particle types, a thermostat. Particles with a type less than MAX_STATIC_TYPE are not moved and have no velocity
 *
 * @see Physics::calculateV
 * @see Physics::calculateX
 * @see Physics::LennardJones::fastForce
 */
class NanoScaleSimulation : public ThermostatSimulation {
 public:
  /** Particles with a type less or equal to this will not be moved */
  const static int MAX_STATIC_TYPE = 1;
  /** Amount of bins to use in calculateStatistics */
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
   * @copydoc ThermostatSimulation::updateX()
   */
  void updateX() override;

  /**
   * @copydoc ThermostatSimulation::updateV()
   */
  void updateV() override;

  /**
   * @copydoc ThermostatSimulation::updateF()
   */
  void updateF() override;

  /**
   * @copydoc ThermostatSimulation::initializeBrownianMotionWithTemperature()
   */
  void initializeBrownianMotionWithTemperature(const double init_temperature) override;

  /**
   * @brief Calculates density and velocity profile
   *
   * Calculates the amount of particles and the average y velocity for each bin along the X axis
   * @param filename Filename to export the csv to
   */
  void calculateStatistics(const std::filesystem::path &filename);
};
