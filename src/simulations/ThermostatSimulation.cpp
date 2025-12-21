//
// Created by jannik on 12/17/25.
//
#include "simulations/ThermostatSimulation.h"

#include "Physics.h"

void ThermostatSimulation::updateV() {
  for (auto &particle : linkedCells.particles) {
    if (particle.getState() < 0) continue;
    spdlog::trace("Updating V:");
    spdlog::trace("-> Old Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
    particle.setV(Physics::StoermerVerlet::velocity(particle, delta_t));
    spdlog::trace("-> New Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
  }
  if (current_iteration % thermostat.getN() == 0) {
    thermostat.updateTemperature(alive_particles);
    spdlog::info("Updated Temperature");
  }
}