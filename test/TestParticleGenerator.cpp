
#include <gtest/gtest.h>

#include "ParticleGenerator.h"

/**
 * @test Basic Cuboid generation
 *
 * This test checks if the particle generator is able to generate a 10x10x10 cuboid with distance 1;
 * @todo This only works with cubes, since we don't have a fixed order for the particles. To test arbitrary cuboids,
 * one would need to search through the particle vector.
 */
TEST(GenerateCuboid, Basic10x10x10) {
  constexpr std::array<unsigned int, 3> size = {10, 10, 10};
  std::vector<Particle> particles;

  ParticleGenerator::cuboid(particles, {0, 0, 0}, size, 1, 0, {0, 0, 0});

  constexpr int N = size[0] * size[1] * size[2];  // std::accumulate is only constexpr >= C++20
  EXPECT_EQ(particles.size(), N);

  unsigned int i = 0;
  for (unsigned int x = 0; x < size[0]; x += 1)
    for (unsigned int y = 0; y < size[1]; y += 1)
      for (unsigned int z = 0; z < size[2]; z += 1) {
        Vector3 p = particles[i].getX();
        EXPECT_DOUBLE_EQ(p[0], x);
        EXPECT_DOUBLE_EQ(p[1], y);
        EXPECT_DOUBLE_EQ(p[2], z);

        if (i++ >= N) {
          FAIL();
        }
      }
}

/**
 * @test Cuboid mass
 *
 * Tests if all the particles have the correct mass
 *
 */
TEST(GenerateCuboid, Mass) {
  std::vector<Particle> particles;
  const double mass = 1.773;
  ParticleGenerator::cuboid(particles, {0, 0, 0}, {5, 5, 5}, 1, mass, {0, 0, 0});

  for (auto &p : particles) {
    EXPECT_DOUBLE_EQ(mass, p.getM());
  }
}

/**
 * @test Cuboid velocity
 *
 * Tests if all the particles have the correct velocity
 *
 */
TEST(GenerateCuboid, Velocity) {
  std::vector<Particle> particles;
  const Vector3 velocity = {55.229, 12.8, -99.9};
  ParticleGenerator::cuboid(particles, {0, 0, 0}, {5, 5, 5}, 1, 0, velocity);

  for (auto &p : particles) {
    EXPECT_EQ(velocity, p.getV());
  }
}
