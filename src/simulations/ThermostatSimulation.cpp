//
// Created by jannik on 12/17/25.
//
#include "simulations/ThermostatSimulation.h"

#include "Physics.h"

void ThermostatSimulation::updateV() {
  linkedCells.applyToParticles([this](Particle p) { p.setV(Physics::StoermerVerlet::velocity(p, delta_t)); });

  if (current_iteration % thermostat.getN() == 0) {
    thermostat.updateTemperature(linkedCells.alive_particles);
    spdlog::info("Updated Temperature");
  }
}