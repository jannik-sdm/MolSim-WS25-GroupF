#pragma once

#include <gtest/gtest.h>

#include "container/directSum/DirectSum.h"
#include "container/directSum/ParticleContainer.h"

class TestDirectSum : public ::testing::Test {
 protected:
  std::unique_ptr<DirectSum> container;
  std::vector<Particle> particles;
  static const unsigned int NUM_PARTICLES = 10;

 public:
  TestDirectSum();
};
