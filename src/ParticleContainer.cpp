#include "ParticleContainer.h"

std::pair<Particle &, Particle &> PairIterator::operator*() { return {particles[i], particles[j]}; }

PairIterator &PairIterator::operator++() {
  j += 1;
  if (j >= particles.size()) {
    i += 1;
    j = i + 1;
  }

  return *this;
}

bool PairIterator::operator==(const PairIterator &other) const { return i == other.i && j == other.j; }

bool PairIterator::operator!=(const PairIterator &other) const { return i < other.i || j < other.j; }
