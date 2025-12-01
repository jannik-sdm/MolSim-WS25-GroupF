//
// Created by jannik on 11/23/25.
//
#pragma once

#include <cmath>
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
  bool is2D;
  /**
   * Distance for when two particles are repulsing
   */
  const double repulsing_distance;
  LinkedCells linkedCells;
  std::vector<Particle> &particles;

 public:
  CutoffSimulation(std::vector<Particle> &particles, Vector3 dimension, double end_time, double delta_t,
                   double cutoffRadius, std::array<BorderType,6>& border, bool is2D);

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

  /**
   * @brief Adds ghost particles to the ghost cells adjacent to the reflective borders of the current border cell
   * @param particle Particle for which we have to create ghost particles
   * @param cell_index Index to the cell the particle is located in
   * @param cell Reference of the cell the particle is located in
   */
  void createGhostParticles(Particle &particle, const int cell_index, Cell &cell);

  /**
   * @brief Creates ghost particles for all particles located in border cells and creates pointers to acces them
   */
  void updateGhost();

  /**
   * @brief Moves the particles that left a cell into their new cell
   */
  void moveParticles();

  /**
   * @brief Initializes the particles with the brownian motion
   */
  void initializeBrownianMotion();
};