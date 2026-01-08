//
// Created by jannik on 11/6/25.
//

#pragma once
#include <spdlog/spdlog.h>

/**
 * @brief Base class for simulations
 *
 * This class ensures that every simulation has a iterate method
 */
class Simulation {
 protected:
  /**
   * Start time of the simulation
   */
  const double start_time;
  /**
   * End time of the simulation
   */
  const double end_time;
  /**
   * timestep of the simulation
   */
  const double delta_t;
  /**
   * Current iteration the simulation is in
   */
  unsigned int current_iteration;

 public:
  /**
   * Initializes the basic parameters of a simulation
   * @param start_time start time of the simulation
   * @param end_time end time of the simulation
   * @param delta_t timestep of the simulation
   */
  Simulation(const double start_time, const double end_time, const double delta_t)
      : start_time(start_time), end_time(end_time), delta_t(delta_t), current_iteration(0) {}
  // destructor to avoid memory leaks
  virtual ~Simulation() = default;
  friend class TestThermostatSimulation;

  /**
   * @brief Starts the iterations of a simulation, applying the given function periodically
   * @tparam Function a function template to add logic to the basic implementation
   * @param f a function to add logic to the basic implementation
   *
   */
  template <typename Function>
  void run(Function f) {
    double current_time = start_time;
    current_iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
      iteration();

      f(current_iteration);
#ifndef ENABLE_TIME_MEASURE
      spdlog::info("Iteration {} finished.", current_iteration);
#endif

      current_time += delta_t;
      current_iteration++;
    }
  };

  /**
   * @brief Performs one complete iteration of the simulation.
   *
   * This is a virtual function defining the core simulation step
   * for all concrete simulation types. Derived classes must implement
   * this method to execute one complete cycle of the respective simulation logic.
   */
  virtual void iteration() = 0;
  /**
   * @brief Updates the position of each particle
   */
  virtual void updateX() = 0;
  /**
   * @brief Updates the velocity of each particle
   */
  virtual void updateV() = 0;
  /**
   * @brief Updates the force of each particle
   */
  virtual void updateF() = 0;
};
