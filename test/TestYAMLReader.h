#pragma once

#include <gtest/gtest.h>

#include "Particle.h"
#include "ParticleGenerator.h"
#include "Settings.h"

class TestYAMLReader : public ::testing::Test {
 protected:
  std::vector<Particle> particles;
  Settings settings = Settings(particles);

  TestYAMLReader() = default;
};
