//
// Created by jannik on 12/14/25.
//
#pragma once
#include <vector>

#include "../Particle.h"
class Thermostat {
  /**
   * Reference to the particles of the simulation
   */
  std::vector<Particle> &particles;
  /**
   * Reference to the alive_particles field in the simulation to keep track of amount of particles still alive
   */
  int &alive_particles;

  bool is2D;

  /**
   * @brief The current temperature of the system
   */
  double current_temperature;
  /**
   * The target temperature the simulation should reach after applying the thermostat (potentially many times)
   */
  double target_temperature;
  /**
   * The maximum amount the temperature is allowed to change in each application of the thermostat
   */
  double maximum_temperature_change;

  /**
   * The number of timesteps after which the thermostat is periodically applied
   */
  int n;

 public:
  Thermostat(std::vector<Particle> &particles, int n, double target_temperature, double maximum_temperature_change,
             int &alive_particles, bool is2D, double initial_temperature = -0.1,
             double average_brownian_velocity = -0.1);

  /**
   * @brief Calculates the current temperature of the simulation
   * @return current temperature of the simulation
   */
  double calculateCurrentTemperature();

  /**
   * @brief Calculates the scaling factor beta based on the target temperature
   * @return scaling factor beta
   */
 private:
  double calculateScalingFactor();

  /**
   * @brief Applies the scaling factor beta to each velocity
   * @param scaling_factor scaling factor beta applied to each velocity
   */
  void applyScalingFactor(double scaling_factor);

  /**
   * @brief Calculates the kinetic energy of the simulation (sum of energy of all particles)
   * @return Sum of the kinetic energy of all the particles
   */
  double calculateEkin();

  /**
   * @brief initializes the simulation with the brownian motion, if the velocities are zero
   */
  void initializeBrownianMotionZero(double initial_temperature);

  /**
   * @brief initializes the simulation with the brownian motion, if the velocities are non-zero
   */
  void initializeBrownianMotion(double brownian_motion_avg_velocity);
};
