#include "ParticleContainer.h"

#include <iostream>

std::pair<Particle &, Particle &> ParticleContainer::PairIterator::operator*() { return {particles[i], particles[j]}; }

ParticleContainer::PairIterator &ParticleContainer::PairIterator::operator++() {
  j += 1;
  if (j >= particles.size()) {
    i += 1;
    j = i + 1;
  }

  return *this;
}

bool ParticleContainer::PairIterator::operator==(const PairIterator &other) const {
  return i == other.i && j == other.j;
}

bool ParticleContainer::PairIterator::operator!=(const PairIterator &other) const { return i < other.i || j < other.j; }
