//
// Created by jannik on 12/3/25.
//
#pragma once
#include "DirectSum.h"
void DirectSum::applyToAllPairs(std::function<Vector3(Particle &p1, Particle &p2)> calculateForce) {
  for (int i = 0; i < particles.size(); i++) {
    for (int j = i + 1; j < particles.size(); j++) {
      Particle &p1 = particles[i];
      Particle &p2 = particles[j];
      Vector3 f = calculateForce(p1, p2);
      p1.addF(f);
      p2.subF(f);
    }
  }
}

void DirectSum::applyToAllParticles(std::function<Vector3(Particle &)> apply, int k) {
  for (auto &p : particles) {
    apply(p);
  }
}

void DirectSum::updatePosition(Vector3 updateX(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    updateX(p, delta_t);
  }
}

void DirectSum::updateVelocity(Vector3 calculateV(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    calculateV(p, delta_t);
  }
}
