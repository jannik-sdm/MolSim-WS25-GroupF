//
// Created by jannik on 11/23/25.
//
#pragma once

#include <memory>

#include "../LinkedCells/LinkedCells.h"
#include "Simulation.h"

class CutoffSimulation : public Simulation {
 private:
  const double epsilon = 5;
  const double sigma = 1;
  const double brownian_motion_avg_velocity = 0.1;
  const double end_time;
  const double delta_t;
  const double cutoffRadius = 3.0;
  LinkedCells linkedCells;
  std::vector<Particle> &particles;

 public:
  CutoffSimulation(std::vector<Particle> &particles, Vector3 dimension, double end_time, double delta_t,
                   double cutoffRadius);

  /**
   * Performes one iteration of the simulation by updating the force, position and velocity of each particle
   */
  void iteration() override;

  /**
   * Calculates the forces between each particle up to a specified cutoff radius and updates them
   */
  void updateF() override;
  /**
   * Updates the position of each particle and moves the pointers from cell to cell
   */
  void updateX() override;
  /**
   * Updates the velocity of each particle
   */
  void updateV() override;

  void createGhostParticles(int particle_index, Cell &current_cell, int current_cell_index, Cell &new_cell);
  void moveParticles();

  void initializeBrownianMotion();
};