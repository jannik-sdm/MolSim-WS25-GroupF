//
// Created by jannik on 12/3/25.
//
#pragma once
#include "DirectSum.h"

void DirectSum::applyToAllPairs(void applyForce(Particle &p1, Particle &p2, double sigma, double epsilon)) {
  for (int i = 0; i < particles.size(); i++) {
    for (int j = i + 1; j < particles.size(); j++) {
      if (i >= j) continue;
      applyForce(particles[i], particles[j], sigma, epsilon);
    }
  }
}

void DirectSum::applyToAllParticles(std::function<void(Particle &)> apply) {
  for (auto &p : particles) {
    apply(p);
  }
}

void DirectSum::updatePosition(void updateX(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    updateX(p, delta_t);
  }
}

void DirectSum::updateVelocity(void updateV(Particle &p, double delta_t)) {
  for (auto &p : particles) {
    updateV(p, delta_t);
  }
}
