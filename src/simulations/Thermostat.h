//
// Created by jannik on 12/14/25.
//
#pragma once
#include <cmath>
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
             bool is2D, double initial_temperature = -0.1, double average_brownian_velocity = -0.1);
  /**
   * @brief Calculates the current temperature of the simulation
   * @return current temperature of the simulation
   */
  double calculateCurrentTemperature();

  /**
   * @brief Performs one update of the velocities to change the temperature to the target temperature
   */
  void updateTemperature();

  /**
   *
   * @return getter for attribute n
   */
  int getN() {return n;}


 private:
  /**
   * @brief Calculates the scaling factor beta based on the target temperature
   * @return scaling factor beta
   */
  double calculateScalingFactor();

  /**
   * @brief Calculates the maximum possible scaling factor that the simulation is able to use
   * @return scaling factor beta_max
   */
  double calculateMaximumScalingFactor();

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

  /**
   *
   * @return Count of alive particles
   */
  int calculateAliveParticles();

};
