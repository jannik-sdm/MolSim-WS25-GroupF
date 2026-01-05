// https://google.github.io/googletest/primer.html

#include "TestDirectSum.h"

TestDirectSum::TestDirectSum() {
  for (int i = 0; i < NUM_PARTICLES; i++) {
    std::array<double, 3> v = {0, 0, 0};
    std::array<double, 3> x = {0, 0, 0};
    double m = i;

    particles.emplace_back(x, v, m);
  }
  container = std::make_unique<DirectSum>(particles);
}

/**
 * @test Apply to all particles
 *
 * This test case checks if the container applies a given function to every particle
 *
 */
TEST_F(TestDirectSum, ApplyToParticles) {
  container->applyToParticles([](Particle &p) {
    Vector3 v = {0, 1, 1};
    p.setV(v);
  });
  int i = 0;
  Vector3 v = {0, 1, 1};
  for (Particle &particle : particles) {
    EXPECT_EQ(particle.getV()[0], v[0]);
    EXPECT_EQ(particle.getV()[1], v[1]);
    EXPECT_EQ(particle.getV()[2], v[2]);

    if (i++ >= NUM_PARTICLES) {
      FAIL();
    }
  }
}

/**
 * @test ApplyToPairs
 *
 * This test case checks wether the pair iteration works.
 * For a given vector, it should iterate over all unique pairs, and apply the function f
 * e.g
 * ```
 * (0, 1, 2, 3) -> [0, 1] [0, 2] [0, 3] [1, 2] [1, 3] [2, 3]
 * ```
 */
TEST_F(TestDirectSum, ApplyToPairsWorks) {
  container->applyToPairs([](Particle &p1, Particle &p2) {
    p1.addF({1, 0, 0});
    p2.addF({1, 0, 0});
  });

  for (Particle &p : particles) {
    int f1 = p.getF()[0];
    EXPECT_EQ(f1, NUM_PARTICLES - 1);
  }
}
