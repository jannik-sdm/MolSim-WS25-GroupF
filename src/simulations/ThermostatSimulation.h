#pragma once

#include "Particle.h"
#include "container/linkedCells/Cell.h"
#include "simulations/CutoffSimulation.h"
#include "simulations/Thermostat.h"

/**
 * Struct to save the precomputed sigma and epsilon values for to particle types
 */
struct interactionParams {
  double sigma2;
  double epsilon24;  // 24 * epsilon (saves multiplication later)
};

/**
 * Struct to save the sigma and epsilon values of a particle type
 */
struct sigma_epsilon {
  double sigma;
  double epsilon;

  // This is required for std::map to work
  bool operator<(const sigma_epsilon &other) const {
    // First compare sigma
    if (sigma != other.sigma) {
      return sigma < other.sigma;
    }
    // If sigmas are equal, compare epsilon
    return epsilon < other.epsilon;
  }
};
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
class ThermostatSimulation : public CutoffSimulation {
 protected:
  /**
   * Thermostat that implements several methods to control the temperature of the system
   */
  Thermostat &thermostat;
  /**
   * Map that maps a sigma and epsilon struct to a particle type
   */
  std::map<sigma_epsilon, int> particle_types;
  /**
   * Amount of different particle types there are in the simulation
   */
  int num_types = 0;
  /**
   * A 1D vector that contains the precomputed sigma² and 24*epsilon values of the different particle type pairs
   * Access patter: mixing_table[p1.getType() * num_types + p2.getType()
   */
  std::vector<interactionParams> mixing_table;

 public:
  ThermostatSimulation(LinkedCells &linkedCells, const double start_time, const double end_time, const double delta_t,
                       const std::optional<double> brown_motion_avg_velocity, const Vector3 &dimension,
                       const double cutoff_radius, const std::array<BorderType, 6> &border, const bool is2D,
                       const double g_grav, const std::optional<double> t_initial, Thermostat &thermostat)
      : CutoffSimulation(linkedCells, start_time, end_time, delta_t, brown_motion_avg_velocity, dimension,
                         cutoff_radius, border, is2D, g_grav),
        thermostat(thermostat) {
    // also sets types of the particles
    initializeParticleTypes();
    initializeMixingTable();
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

  /**
   * Calculates the forces between each particle up to a specified cutoff radius and updates them
   */
  void updateF() override;

  /**
   * Initializes the system with the brownian motion, based on the given initial temperature
   * @param init_temperature The initial temperature of the system
   */
  virtual void initializeBrownianMotionWithTemperature(const double init_temperature);

  /**
   * Scans the particles, sets their type and initializes the types vector
   */
  void initializeParticleTypes();

  /**
   *
   * @param sigma sigma of the particle
   * @param epsilon epsilon of the particle
   * @return type of the particle
   */
  int getTypeId(double sigma, double epsilon);

  /**
   * Precomputes a mixing table containing values needed for the calculation of the forces for two particles of
   * different type
   */
  void initializeMixingTable();
};
