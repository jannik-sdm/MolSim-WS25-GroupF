#pragma once

#include <gtest/gtest.h>

#include "container/directSum/ParticleContainer.h"

class TestParticleContainer : public ::testing::Test {
 protected:
  std::unique_ptr<ParticleContainer> container;
  std::vector<Particle> particles;
  static const unsigned int NUM_PARTICLES = 10;

 public:
  TestParticleContainer();
};
