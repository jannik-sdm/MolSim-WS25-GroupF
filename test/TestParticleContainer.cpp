// https://google.github.io/googletest/primer.html

#include "ParticleContainer.h"
#include "gtest/gtest.h"
class TestParticleContainer : public ::testing::Test {
 protected:
  ParticleContainer container;
  static const unsigned int NUM_PARTICLES = 10;

  TestParticleContainer() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
      std::array<double, 3> v = {static_cast<double>(i * 3 % 7), static_cast<double>(i * 5 % 8) / 3,
                                 static_cast<double>((i + 12) * 3)};
      std::array<double, 3> x = {static_cast<double>(i * 3 % 7), static_cast<double>(i % 3 + 4) / 2,
                                 static_cast<double>((i + 81) * 12 % 41) * 2.4};
      double m = static_cast<double>(i);

      container.particles.emplace_back(x, v, m, i);
    }
  }
};

/**
 * @test Linear iteration
 *
 * This test case checks if the container can be iterated through linearly in order
 *
 */
TEST_F(TestParticleContainer, IterationWorks) {
  int i = 0;
  for (Particle &particle : container) {
    EXPECT_EQ(particle.getType(), i);

    if (i++ >= NUM_PARTICLES) {
      FAIL();
    }
  }
}

