//
// Created by jv_fedora on 19.11.25.
//

#include "TestLinkedCells.h"

#include <algorithm>
#include <memory>
#include <vector>


#include "utils/ArrayUtils.h"

// Define a Fixture to reduce code duplication
TestLinkedCells::TestLinkedCells() {
    // Helper to create a LinkedCells object with specific parameters
    // Domain: 3.0 x 3.0 x 3.0
    // Cutoff: 1.0
    // Expected Inner Grid: 3x3x3
    // Expected Total Grid (with ghosts): 5x5x5 = 125 cells
    std::unique_ptr<LinkedCells> createTestContainer(std::vector<Particle> particles) {
        Vector3 domain = {3.0, 3.0, 3.0};
        double cutoff = 1.0;
        return std::make_unique<LinkedCells>(std::move(particles), domain, cutoff);
    }
};

/**
 * @brief Checks if the grid dimensions are calculated correctly including Ghost Layers.
 * Domain 3x3x3, Cutoff 1.0 -> 3 inner cells.
 * Ghost layers +2 -> 5 total cells per dimension.
 * Total cells = 5 * 5 * 5 = 125.
 */
TEST_F(LinkedCellsTest, GridDimensionsCalculation) {
    std::vector<Particle> particles;
    auto container = createTestContainer(std::move(particles));

    EXPECT_EQ(container->cells.size(), 125);

    // Check specific dimension variables if accessible
    // EXPECT_EQ(container->numCellsX, 5);
}

/**
 * @brief Verifies that Cell Types (Ghost, Border, Inner) are assigned correctly.
 * In a 5x5x5 grid (indices 0..4):
 * - 0 and 4 are GHOST
 * - 1 and 3 are BORDER
 * - 2 is REGULAR (Inner)
 */
TEST_F(LinkedCellsTest, CellTypeAssignment) {
    std::vector<Particle> particles;
    auto container = createTestContainer(std::move(particles));

    // 1. Check a Ghost Cell (0, 0, 0)
    int ghostIdx = container->index3dToIndex1d(0, 0, 0);
    EXPECT_EQ(container->cells[ghostIdx].cell_type, CellType::GHOST);

    // 2. Check a Border Cell (1, 1, 1) -> This is the first real cell
    int borderIdx = container->index3dToIndex1d(1, 1, 1);
    EXPECT_EQ(container->cells[borderIdx].cell_type, CellType::BORDER);

    // 3. Check the Center Cell (2, 2, 2) -> Should be REGULAR
    int innerIdx = container->index3dToIndex1d(2, 2, 2);
    EXPECT_EQ(container->cells[innerIdx].cell_type, CellType::REGULAR); // Assuming REGULAR is default/enum

    // 4. Check upper boundary Ghost (4, 4, 4)
    int ghostUpper = container->index3dToIndex1d(4, 4, 4);
    EXPECT_EQ(container->cells[ghostUpper].cell_type, CellType::GHOST);
}

/**
 * @brief Tests the mapping from a Coordinate (double) to a 1D Index (int).
 * Critical for placing particles correctly.
 */
TEST_F(LinkedCellsTest, CoordinateToIndexMapping) {
    std::vector<Particle> particles;
    auto container = createTestContainer(std::move(particles));

    // Domain starts at 0.0. Cell size is 1.0.
    // Index 0 is Ghost. Index 1 is Real 0.0 to 1.0.

    // Test 1: Coordinate 0.5 (Inside first real cell)
    // Expected 3D Index: (1, 1, 1)
    int idx1 = container->coordinate3dToIndex1d(0.5, 0.5, 0.5);
    int expected1 = container->index3dToIndex1d(1, 1, 1);
    EXPECT_EQ(idx1, expected1);

    // Test 2: Coordinate 1.5 (Inside second real cell)
    // Expected 3D Index: (2, 2, 2)
    int idx2 = container->coordinate3dToIndex1d(1.5, 1.5, 1.5);
    int expected2 = container->index3dToIndex1d(2, 2, 2);
    EXPECT_EQ(idx2, expected2);
}

/**
 * @brief Tests 3D to 1D index conversion arithmetic.
 * x + dimX*y + dimX*dimY*z
 */
TEST_F(LinkedCellsTest, IndexArithmetic) {
    std::vector<Particle> particles;
    auto container = createTestContainer(std::move(particles));

    // Stride X should be 5
    // Stride Y should be 5
    // Index (1, 2, 3) = 1 + (5*2) + (25*3) = 1 + 10 + 75 = 86
    int calculated = container->index3dToIndex1d(1, 2, 3);
    EXPECT_EQ(calculated, 86);
}

/**
 * @brief Tests the Loop logic within getNeighbourCells.
 * Ensure it retrieves 26 neighbours.
 */
TEST_F(LinkedCellsTest, NeighborSearch) {
    std::vector<Particle> particles;
    auto container = createTestContainer(std::move(particles));

    // Pick the center cell (2, 2, 2)
    int centerIdx = container->index3dToIndex1d(2, 2, 2);

    std::array<int, 26> neighbors = container->getNeighbourCells(centerIdx);

    // 1. Verify we don't return the cell itself
    for(int idx : neighbors) {
        EXPECT_NE(idx, centerIdx);
    }

    // 2. Verify a specific known neighbor exists
    // (2,2,2) should have (1,1,1) as a neighbor
    int knownNeighbor = container->index3dToIndex1d(1, 1, 1);

    bool found = false;
    for(int idx : neighbors) {
        if(idx == knownNeighbor) found = true;
    }
    EXPECT_TRUE(found) << "Neighbor list did not contain index (1,1,1)";
}

/**
 * @brief Tests that particles passed to constructor end up in the vector of the correct cell.
 */
TEST_F(LinkedCellsTest, ParticleBinning) {
    std::vector<Particle> particles;
    // P1 at 0.5 -> Cell (1,1,1)
    Particle p1({0.5, 0.5, 0.5}, {0,0,0}, 1, 0);
    // P2 at 2.5 -> Cell (3,3,3)
    Particle p2({2.5, 2.5, 2.5}, {0,0,0}, 1, 0);

    particles.push_back(p1);
    particles.push_back(p2);

    auto container = createTestContainer(std::move(particles));

    // Check Cell 1 (1,1,1)
    int cell1Index = container->index3dToIndex1d(1, 1, 1);
    EXPECT_EQ(container->cells[cell1Index].particles.size(), 1);
    EXPECT_EQ(container->cells[cell1Index].particles[0]->getX()[0], 0.5);

    // Check Cell 2 (3,3,3)
    int cell2Index = container->index3dToIndex1d(3, 3, 3);
    EXPECT_EQ(container->cells[cell2Index].particles.size(), 1);
}