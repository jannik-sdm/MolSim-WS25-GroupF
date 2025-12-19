//
// Created by jannik on 12/17/25.
//

#include "simulations/ThermostatSimulation.h"

void iteration() {}

void updateV() {
  for (auto &particle : linkedCells.particles) {
    if (particle.getState() < 0) continue;
    spdlog::trace("Updating V:");
    spdlog::trace("-> Old Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);

    double beta = 1;
    if (thermostat.n % 100 == 0) beta = 1;  // faktor aus thermostat.

    particle.setV(beta * Physics::StoermerVerlet::velocity(particle, delta_t));
    spdlog::trace("-> New Velocity: ({},{},{})", particle.getV()[0], particle.getV()[1], particle.getV()[2]);
  }
}