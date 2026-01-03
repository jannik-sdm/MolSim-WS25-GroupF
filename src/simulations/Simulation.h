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
  const double start_time;
  const double end_time;
  const double delta_t;
  unsigned int current_iteration;

 public:
  Simulation(const double start_time, const double end_time, const double delta_t)
      : start_time(start_time), end_time(end_time), delta_t(delta_t), current_iteration(0) {}
  // destructor to avoid memory leaks
  virtual ~Simulation() = default;

  template <typename Function>
  void run(Function f) {
    double current_time = start_time;
    current_iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < end_time) {
      iteration();

      f(current_iteration);
      spdlog::info("Iteration {} finished.", current_iteration);

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
  virtual void updateX() = 0;
  virtual void updateV() = 0;
  virtual void updateF() = 0;
};
