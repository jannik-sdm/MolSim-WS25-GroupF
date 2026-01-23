//
// Created by jannik on 12/17/25.
//
#include "simulations/nanoScale/NanoScaleSimulation.h"

#include <fstream>

#include "simulations/Physics.h"
#include "utils/MaxwellBoltzmannDistribution.h"

void NanoScaleSimulation::updateX() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getState() < 0) return;
    if (p.getType() < 1) return;

    p.setX(Physics::StoermerVerlet::position(p, delta_t));
  });

  linkedCells.moveParticles();
}

void NanoScaleSimulation::updateV() {
  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getType() < MAX_STATIC_TYPE) return;
    p.setV(Physics::StoermerVerlet::velocity(p, delta_t));
  });

  if (current_iteration % thermostat.getN() == 0 && current_iteration > 0) {
    thermostat.updateTemperature(linkedCells.alive_particles);
    spdlog::info("Updated Temperature");
  }
}

void NanoScaleSimulation::updateF() {
  // set the force of all particles to zero
  linkedCells.applyToParticles([this](Particle &p) { p.setF({0, g_grav * p.getM(), 0}); });

  linkedCells.applyToPairs([this](Particle &p1, Particle &p2) {
    interactionParams params = mixing_table[p1.getType() * num_types + p2.getType()];

    Vector3 f = Physics::LennardJones::fastForce(p1, p2, params.sigma2, params.epsilon24);
    p1.addF(f);
    p2.subF(f);
  });

  linkedCells.applyToParticles([this](Particle &p) {
    if (p.getType() < MAX_STATIC_TYPE) p.setF({0, 0, 0});
  });
}

void NanoScaleSimulation::initializeBrownianMotionWithTemperature(const double init_temperature) {
  linkedCells.applyToParticles([this, init_temperature](Particle &p) {
    if (p.getType() < MAX_STATIC_TYPE) return;
    p.setV(p.getV() + maxwellBoltzmannDistributedVelocity(sqrt(init_temperature / p.getM()), (is2D ? 2 : 3)));
  });
}

void NanoScaleSimulation::calculateStatistics(std::filesystem::path &filename) {
  // Count per bin, average x-velocity
  std::vector<std::pair<unsigned int, double>> bins;
  bins.resize(BINS);

  linkedCells.applyToParticles([this, &bins](Particle &p) {
    const unsigned int bin = p.getX()[0] / BINS;

    if (bin < 0 || bin > bins.size()) return;

    auto &b = bins.at(bin);

#pragma atomic
    b.first++;

    /**
     * \f$ m_n = m_{n-1} + \frac{a_n - m_{n_1}}{n} \f$
     *
     */
#pragma atomic
    b.second += (p.getX()[0] - b.second) / b.first;
  });

  // Output
  std::ofstream file(filename);
  for (auto &bin : bins) {
    file << bin.first << ',' << bin.second << '\n';
  }
}
