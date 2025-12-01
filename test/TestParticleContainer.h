#pragma once

#include "ParticleContainer.h"
#include "gtest/gtest.h"

class TestParticleContainer : public ::testing::Test {
 protected:
  std::unique_ptr<ParticleContainer> container;
  std::vector<Particle> particles;
  static const unsigned int NUM_PARTICLES = 10;

 public:
  TestParticleContainer();
};
