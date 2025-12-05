//
// Created by jannik on 12/2/25.
//
#include "TestCutoffSimulation.h"

#include <spdlog/spdlog.h>

#include "Particle.h"
#include "utils/ArrayUtils.h"

/**
 * A particle close to the left boundary (x=0) should create a ghost
 * with inverted X velocity and mirrored X position.
 */
TEST_F(CutoffSimulationTest, CreatesGhostAtReflectiveBoundary) {
  // Place particle at x=0.1 (very close to left border at x=0)
  // y=5.0, z=5.0 (center of face)
  // Velocity = (-1, 0, 0) moving towards the wall
  Vector3 pos = {0.1, 5.0, 5.0};
  Vector3 vel = {-1.0, 0.0, 0.0};
  particles.emplace_back(pos, vel, 1.0, 0);

  InitSimulation();

  sim->updateGhost();

  // Calculate where the ghost SHOULD be
  // Ghost X = 0.0 - 0.1 = -0.1
  Vector3 expectedGhostPos = {-0.1, 5.0, 5.0};

  // check what velocity should be after brownian motion is initialized
  Vector3 expectedVelocity = particles[0].getV();

  // B. Find the Ghost Cell Index
  int ghostCellIndex = sim->getLinkedCells().coordinate3dToIndex1d(expectedGhostPos);

  // C. Access that cell
  Cell &ghostCell = sim->getLinkedCells().cells[ghostCellIndex];

  // D. Checks
  EXPECT_EQ(ghostCell.cell_type, CellType::GHOST) << "Target cell must be a Ghost Cell";
  ASSERT_EQ(ghostCell.size_ghost_particles, 1) << "One ghost particle should have been created";

  // E. Verify Properties
  Particle &ghost = ghostCell.ghost_particles[0];

  // Position Check
  EXPECT_NEAR(ghost.getX()[0], expectedGhostPos[0], 1e-5);
  EXPECT_NEAR(ghost.getX()[1], expectedGhostPos[1], 1e-5);

  // Velocity Check (X should be inverted:
  EXPECT_EQ(ghost.getV()[0], -expectedVelocity[0]);
  EXPECT_EQ(ghost.getV()[1], expectedVelocity[1]);
}

/**
 * A particle inside a border cell, but NOT close enough to the wall
 * (distance > repulsing_distance) should NOT create a ghost.
 */
TEST_F(CutoffSimulationTest, IgnoresParticlesTooFarFromBorder) {
  // Sigma = 1.0 --> Repulsing distance is ~1.12.
  // Particle needs to be within 0.56 of the wall (2*dist < 1.12).
  // Let's place it at x=2.0 (Inside border cell [0, 2.5], but far from wall).

  Vector3 pos = {2.0, 5.0, 5.0};
  Vector3 vel = {0.0, 0.0, 0.0};
  particles.emplace_back(pos, vel, 1.0, 0);

  InitSimulation();
  sim->updateGhost();

  // Calculate Ghost Cell Index (Left side)
  Vector3 ghostRegion = {-1.0, 5.0, 5.0};
  int ghostCellIndex = sim->getLinkedCells().coordinate3dToIndex1d(ghostRegion);

  Cell &ghostCell = sim->getLinkedCells().cells[ghostCellIndex];

  // Should be 0 because it's too far to exert force
  EXPECT_EQ(ghostCell.size_ghost_particles, 0);
}

/**
 * A particle in a corner (close to x=0 AND y=0) should create TWO ghosts.
 */
TEST_F(CutoffSimulationTest, CreatesGhostsInCorner) {
  // Pos close to X=0 and Y=0
  Vector3 pos = {0.1, 0.1, 5.0};
  particles.emplace_back(pos, Vector3{0, 0, 0}, 1.0, 0);

  InitSimulation();
  sim->updateGhost();

  // Check Left Ghost Cell (-0.1, 0.1, 5.0)
  int leftIndex = sim->getLinkedCells().coordinate3dToIndex1d(-0.1, 0.1, 5.0);
  EXPECT_EQ(sim->getLinkedCells().cells[leftIndex].size_ghost_particles, 1) << "Should have Left ghost";

  // Check Bottom Ghost Cell (0.1, -0.1, 5.0)
  int bottomIndex = sim->getLinkedCells().coordinate3dToIndex1d(0.1, -0.1, 5.0);
  EXPECT_EQ(sim->getLinkedCells().cells[bottomIndex].size_ghost_particles, 1) << "Should have Bottom ghost";
}

TEST_F(CutoffSimulationTest, ReflectiveBoundaryGeneratesRepulsiveForce) {
  // Configure REFLECTION boundaries
  borders = {REFLECTION, REFLECTION, REFLECTION, REFLECTION, REFLECTION, REFLECTION};

  // Place a particle very close to the Left Wall (x=0)
  // repulsing_distance is usually approx 1.12 (for sigma=1).
  // Position 0.1 is definitely within range to feel the wall.
  Vector3 pos = {0.1, 5.0, 5.0};
  particles.emplace_back(pos, Vector3{-1.0, 0, 0}, 1.0, 0);  // Moving towards wall

  InitSimulation();
  // Ensure force is zero
  Particle &p = sim->getLinkedCells().particles[0];
  p.setF({0.0, 0.0, 0.0});

  // Update Ghosts and Calculate Forces
  // This creates the ghost at x=-0.1 and calculates LJ force between Real(0.1) and Ghost(-0.1)
  sim->updateGhost();
  sim->updateF();

  // Check for Repulsive Force
  Vector3 f = p.getF();

  // The wall is on the Left (x=0). The particle is at x=0.1.
  // The force should push the particle to the Right (Positive X).
  EXPECT_GT(f[0], 0.0) << "Reflective wall should exert a positive repulsive force on particle at x=0.1";

  // Y and Z forces should remain ~0 since the wall is flat in X
  EXPECT_NEAR(f[1], 0.0, 1e-5);
  EXPECT_NEAR(f[2], 0.0, 1e-5);

  spdlog::info("Repulsive Wall Force calculated: {}", f[0]);
}

TEST_F(CutoffSimulationTest, OutflowBoundaryKillsParticle) {
  // Configure OUTFLOW boundaries
  borders = {OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW, OUTFLOW};

  // Create a particle inside the domain (e.g., x=0.5)
  Vector3 pos = {0.5, 5.0, 5.0};
  particles.emplace_back(pos, Vector3{1.0, 0, 0}, 1.0, 0);  // Moving Right
  InitSimulation();

  // Simulate the particle moving OUT of the domain
  // manually update the position to be in the Ghost Layer (x = -0.5)

  Particle &p = sim->getLinkedCells().particles[0];
  p.setX({-0.5, 5.0, 5.0});

  // Trigger the move logic
  sim->moveParticles();

  // The particle should be marked as Dead (-1)
  EXPECT_EQ(p.getType(), -1) << "Particle leaving the domain should be marked as dead (-1)";
}