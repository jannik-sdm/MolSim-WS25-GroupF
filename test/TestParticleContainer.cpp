// https://google.github.io/googletest/primer.html

#include "ParticleContainer.h"
#include "gtest/gtest.h"
class TestParticleContainer : public ::testing::Test {
  protected:
  ParticleContainer container;
  int num_particles = 4;
  TestParticleContainer() {
    for (int i = 0; i < num_particles ; i++) {
      std::array<double, 3> v = {static_cast<double>(i * 3 % 7), static_cast<double>(i * 5 % 8) /3, static_cast<double>((i + 12) * 3)};
      std::array<double, 3> x = {static_cast<double>(i * 3 % 7), static_cast<double>(i % 3 + 4) /2, static_cast<double>((i + 81) * 12 % 41) *2.4};
      double m = static_cast<double>(i);
      container.particles.emplace_back(x, v, m);
    }
  }
};

TEST_F(TestParticleContainer, IterationWorks) {
  int i = 0;
  for (Particle &particle : container) {
    EXPECT_EQ(container.particles.at(i).getM(), particle.getM());
    i++;
  }
}