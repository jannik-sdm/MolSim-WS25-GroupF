//
// Created by jannik on 11/6/25.
//

#ifndef MOLSIM_WS25_GROUPF_PLANETSIMULATION_H
#define MOLSIM_WS25_GROUPF_PLANETSIMULATION_H
#include "ParticleContainer.h"
#include "Simulation.h"

class PlanetSimulation : public Simulation {
public:

  PlanetSimulation(const ParticleContainer &container, const double end_time, const double delta_t);
  /**
    * Calculates one timestep of the simulation and applies the changes to the particles.
    */
  void iteration() override;

private:
  ParticleContainer particleContainer;
  double end_time;
  double delta_t;

  void calculateX();
  void calculateV();
  void calculateF();
};

#endif  // MOLSIM_WS25_GROUPF_PLANETSIMULATION_H
