//
// Created by jannik on 11/6/25.
//

#pragma once

#include "container/directSum/DirectSum.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "simulations/Simulation.h"

/**
 * @class PlanetSimulation
 * @brief Simulation for Assignment 1
 *
 * This class calculates timesteps for a planet simulation.
 *
 * @see Physics::calculateV
 * @see Physics::calculateX
 * @see Physics::planetForce
 */
class PlanetSimulation : public Simulation {
 protected:
  /** @brief Container for the particles */
  DirectSum container;

 public:
  /**
   * Constructs a planet simulation
   * @param particles reference to the particles array
   * @param start_time start time of the simulation
   * @param end_time end time of the simulation
   * @param delta_t timestep of the simulation
   */
  PlanetSimulation(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t)
      : Simulation(start_time, end_time, delta_t), container(particles) {}
  /**
   * Calculates one timestep of the simulation and applies the changes to the particles.
   */
  void iteration() override;

  /**
   * @brief calculate the force for all particles
   *
   * For each pair of disjunct particles this function calculates the force between the two particles.
   * Then this function sums up all forces for one particle to calculate the effective force of each particle
   */
  virtual void updateF() override;

  /**
   * @brief calculate the position for all particles
   *
   * For each particle i this function calculates the new position x.
   */
  virtual void updateX() override;

  /**
   * @brief calculate the Velocity for all particles
   *
   * For each particle i this function calculates the new Velocity v
   */
  virtual void updateV() override;

  void plotParticles(int iteration, const std::string &filename) override {
#ifdef ENABLE_VTK_OUTPUT
    outputWriter::VTKWriter writer;

    // VTK Filler Lambda
    auto filler = [this](vtkPoints *points, vtkFloatArray *mass, vtkFloatArray *v, vtkFloatArray *f,
                         vtkIntArray *type) {
      container.applyToParticlesPlotting([&](Particle &p) {
        points->InsertNextPoint(p.getX().data());
        mass->InsertNextValue(static_cast<float>(p.getM()));
        v->InsertNextTuple(p.getV().data());
        f->InsertNextTuple(p.getF().data());
        type->InsertNextValue(p.getType());
      });
    };
    writer.plotParticles(filler, filename, iteration);

#else
    outputWriter::XYZWriter writer;

    // XYZ Filler Lambda
    auto filler = [this](std::ofstream &file) {
      container.applyToParticles([&](Particle &p) {
        std::array<double, 3> x = p.getX();
        file << "Ar ";
        file.setf(std::ios_base::showpoint);
        for (auto &xi : x) file << xi << " ";
        file << std::endl;
      });
    };

    int count = container.getAmoutOfParticles();  // Assuming DirectSum has this method
    writer.plotParticles(filler, count, filename, iteration);
#endif
  }
};
