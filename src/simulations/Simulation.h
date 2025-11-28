//
// Created by jannik on 11/6/25.
//

#pragma once

class Simulation {
 public:
  // desctructor to avoid memory leaks
  virtual ~Simulation() = default;

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
