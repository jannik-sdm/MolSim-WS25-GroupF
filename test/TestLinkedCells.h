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
  std::unique_ptr<LinkedCells> linked_cells;
  const double sigma = 1;
  const double repulsing_distance = std::pow(2, 1.0 / 6.0) * sigma;
  std::array<BorderType, 6> borders = {
      BorderType::REFLECTION, BorderType::REFLECTION, BorderType::REFLECTION,
      BorderType::REFLECTION, BorderType::REFLECTION, BorderType::REFLECTION,
  };

  // Constants used for setup
  Vector3 domain = {3.0, 3.0, 3.0};
  double cutoff = 1.0;

  // Runs before each test
  void SetUp() override {
    particles.emplace_back(std::array<double, 3>{0.5, 0.5, 0.5}, std::array<double, 3>{0, 0, 0}, 1.0, 0);
    particles.emplace_back(std::array<double, 3>{2.5, 2.5, 2.5}, std::array<double, 3>{0, 0, 0}, 1.0, 0);

    linked_cells = std::make_unique<LinkedCells>(particles, domain, cutoff, false, borders);
  }

  // Wrapper for index3dToIndex1d
  int callIndex3dToIndex1d(int x, int y, int z) { return linked_cells->index3dToIndex1d(x, y, z); }

  // Wrapper for coordinate3dToIndex1d (takes coords)
  int callCoordinate3dToIndex1d(double x, double y, double z) { return linked_cells->coordinate3dToIndex1d(x, y, z); }

  // Wrapper for getNeighbourCells
  std::array<int, 26> callGetNeighbourCells(int cellIndex) { return linked_cells->getNeighbourCells(cellIndex); }
};
