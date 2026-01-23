//
// Created by jannik on 12/14/25.
//
#include "simulations/nanoScale/NanoScaleThermostat.h"

#include "simulations/nanoScale/NanoScaleSimulation.h"
#include "utils/ArrayUtils.h"
#include "utils/MaxwellBoltzmannDistribution.h"

double NanoScaleThermostat::calculateEkin() {
  double ekin = 0;
  for (auto &p : particles) {
    if (p.getState() < 0) continue;
    if (p.getType() < NanoScaleSimulation::MAX_STATIC_TYPE) continue;

    const double v = ArrayUtils::L2Norm(p.getV());
    ekin += p.getM() * v * v;
  }
  return 0.5 * ekin;
}
