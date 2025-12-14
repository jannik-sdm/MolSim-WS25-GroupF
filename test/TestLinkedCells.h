//
// Created by jannik on 11/26/25.
//
#pragma once

#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "Particle.h"
#include "container/linkedCells/LinkedCells.h"

class TestLinkedCells : public ::testing::Test {
 protected:
  // A. Define the data HERE so it stays alive as long as the test runs
  std::vector<Particle> particles;
  std::unique_ptr<LinkedCells> container;

  // Constants used for setup
  Vector3 domain = {3.0, 3.0, 3.0};
  double cutoff = 1.0;

  // Runs before each test
  void SetUp() override {
    particles.emplace_back(std::array<double, 3>{0.5, 0.5, 0.5}, std::array<double, 3>{0, 0, 0}, 1.0);
    particles.emplace_back(std::array<double, 3>{2.5, 2.5, 2.5}, std::array<double, 3>{0, 0, 0}, 1.0);

    container = std::make_unique<LinkedCells>(particles, domain, cutoff);
  }
};
