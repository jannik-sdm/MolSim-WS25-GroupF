//
// Created by jannik on 12/17/25.
//
#include "simulations/ThermostatSimulation.h"

#include "Physics.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void ThermostatSimulation::updateV() {
  linkedCells.applyToParticles([this](Particle &p) { p.setV(Physics::StoermerVerlet::velocity(p, delta_t)); });

  if (current_iteration % thermostat.getN() == 0 && current_iteration > 0) {
    thermostat.updateTemperature();
    spdlog::info("Updated Temperature to {}", thermostat.calculateCurrentTemperature());
  }
}

void ThermostatSimulation::updateF() {
  // set the force of all particles to zero
  linkedCells.applyToParticles([this](Particle &p) { p.setF({0, g_grav * p.getM(), 0}); });

  linkedCells.applyToPairs([this](Particle &p1, Particle &p2) {
    interactionParams params = mixing_table[p1.getType() * num_types + p2.getType()];

    Vector3 f = Physics::LennardJones::fastForce(p1, p2, params.sigma2, params.epsilon24);
    p1.addF(f);
    p2.subF(f);
  });
}

void ThermostatSimulation::initializeBrownianMotionWithTemperature(const double init_temperature) {
  linkedCells.applyToParticles([this, init_temperature](Particle &p) {
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(sqrt(init_temperature / p.getM()), (is2D ? 2 : 3)));
  });
}

int ThermostatSimulation::getTypeId(double sigma, double epsilon) {
  sigma_epsilon key = {sigma, epsilon};

  // search for the key
  auto it = particle_types.find(key);

  // check if found
  if (it != particle_types.end()) {
    return it->second;
  } else {
    int newId = particle_types.size();
    particle_types[key] = newId;

    return newId;
  }
}

void ThermostatSimulation::initializeParticleTypes() {
  particle_types.clear();
  mixing_table.clear();

  spdlog::info("Initializing Particle Types...");

  linkedCells.applyToParticles([this](Particle &p) { p.setType(getTypeId(p.getSigma(), p.getEpsilon())); });
  num_types = particle_types.size();
}

void ThermostatSimulation::initializeMixingTable() {
  int numTypes = particle_types.size();

  // invert the Map (Map -> Vector)
  std::vector<sigma_epsilon> typeVector(numTypes);

  for (const auto &pair : particle_types) {
    const sigma_epsilon &params = pair.first;
    int id = pair.second;

    typeVector[id] = params;
  }

  mixing_table.resize(numTypes * numTypes);

  for (int i = 0; i < numTypes; ++i) {
    for (int j = 0; j < numTypes; ++j) {
      const double sigma_i = typeVector[i].sigma;
      const double epsilon_i = typeVector[i].epsilon;

      const double sigma_j = typeVector[j].sigma;
      const double epsilon_j = typeVector[j].epsilon;

      const double sigma_mix = Physics::LorentzBerthelot::sigma(sigma_i, sigma_j);
      const double epsilon_mix = Physics::LorentzBerthelot::epsilon(epsilon_i, epsilon_j);

      // precalculate needed parameters
      interactionParams optimizedParams;
      optimizedParams.sigma2 = sigma_mix * sigma_mix;
      optimizedParams.epsilon24 = 24.0 * epsilon_mix;

      // store in the matrix (1d vector)
      mixing_table[i * numTypes + j] = optimizedParams;
    }
  }
}