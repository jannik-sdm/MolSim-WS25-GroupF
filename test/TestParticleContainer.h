#pragma once

#include "ParticleContainer.h"
#include "gtest/gtest.h"

class TestParticleContainer : public ::testing::Test {
 protected:
  ParticleContainer container;
  static const unsigned int NUM_PARTICLES = 10;

 public:
  TestParticleContainer();
};
