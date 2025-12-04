#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Particle.h"
#include "simulations/CollisionSimulation.h"

class CutoffSimulationTest : public ::testing::Test {
 protected:
  std::vector<Particle> particles;
  std::unique_ptr<CollisionSimulation> sim;

  // Simulation Parameters
  Vector3 domain = {10.0, 10.0, 10.0};  // Large enough domain
  double cutoff = 2.5;
  double end_time = 10.0;
  double delta_t = 0.001;
  bool is2D = false;

  // Borders: All Reflective for this test
  std::array<BorderType, 6> borders = {
      REFLECTION, REFLECTION, REFLECTION,  // Left, Bottom, Back
      REFLECTION, REFLECTION, REFLECTION   // Right, Top, Front
  };

  void SetUp() override {
    particles.clear();
    particles.reserve(100);
  }
  void InitSimulation() {
    sim = std::make_unique<CollisionSimulation>(particles, domain, end_time, delta_t, cutoff, borders, is2D);
  }
};
