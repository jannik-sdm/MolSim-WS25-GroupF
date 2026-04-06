#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Particle.h"
#include "simulations/CutoffSimulation.h"

class TestCutoffSimulation : public ::testing::Test {
 protected:
  std::vector<Particle> particles;
  std::unique_ptr<CutoffSimulation> sim;

  // Simulation Parameters
  Vector3 domain = {10.0, 10.0, 10.0};  // Large enough domain
  double cutoff = 2.5;
  double start_time = 0;
  double end_time = 10.0;
  double delta_t = 0.001;
  bool is2D = false;
  double gravity = 0;

  // Borders: All Reflective for this test
  std::array<BorderType, 6> borders = {
      BorderType::REFLECTION, BorderType::REFLECTION,
      BorderType::REFLECTION,  // Left, Bottom, Back
      BorderType::REFLECTION, BorderType::REFLECTION,
      BorderType::REFLECTION  // Right, Top, Front
  };

  void SetUp() override {
    particles.clear();
    particles.reserve(100);
  }
  void initSimulation() {
    sim = std::make_unique<CutoffSimulation>(particles, start_time, end_time, delta_t, std::nullopt, domain, cutoff,
                                             borders, is2D, gravity);
  }

  void callUpdateGhost() { sim->getLinkedCells().updateGhost(); }

  // Wrapper for private coordinate3dToIndex1d
  int callCoordinate3dToIndex1d(double x, double y, double z) {
    return sim->getLinkedCells().coordinate3dToIndex1d(x, y, z);
  }

  // Overload for Vector3
  int callCoordinate3dToIndex1d(Vector3 pos) {
    return sim->getLinkedCells().coordinate3dToIndex1d(pos[0], pos[1], pos[2]);
  }

  // Wrapper for public moveParticles (accessed via getLinkedCells)
  void callMoveParticles() { sim->getLinkedCells().moveParticles(); }
};
