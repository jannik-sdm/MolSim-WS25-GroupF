//
// Created by jannik on 11/23/25.
//
#pragma once

#include <cmath>
#include <memory>

#include "container/linkedCells/LinkedCells.h"
#include "simulations/Simulation.h"
#include "simulations/Thermostat.h"

class CutoffSimulation : public Simulation {
  const double epsilon = 5;
  const double sigma = 1;
  const double brownian_motion_avg_velocity = 0.1;
  const double cutoffRadius = 3.0;
  bool is2D;
  /**
   * Distance for when two particles are repulsing
   */
  const double repulsing_distance;
  LinkedCells linkedCells;
  std::vector<Particle> &particles;

  /**
   * A counter for the amount of particles that are still alive
   */
  int alive_particles = 0;

 public:
  CutoffSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t, const Vector3 &dimension, const double cutoff_radius,
                   const std::array<BorderType, 6> &border, const bool is2D)
      : Simulation(start_time, end_time, delta_t),
        cutoffRadius(cutoff_radius),
        is2D(is2D),
        repulsing_distance(std::pow(2, 1.0 / 6.0) * sigma),
        linkedCells(particles, dimension, cutoff_radius, border),
        particles(particles) {
    for (auto &p : particles)
      if (p.getState() != -1) alive_particles++;
    initializeBrownianMotion();
  }

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
  /**
   * @brief Initializes a thermostat to control the temperature of the simulation
   */
  void addThermostat(int n, double target_temperature, double maximum_temperature_change, double initial_temperature,
                     double average_brownian_velocity);

  /**
   * @brief getter for the tests
   */
  LinkedCells &getLinkedCells() { return linkedCells; }
};
