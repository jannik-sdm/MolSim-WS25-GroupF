// https://google.github.io/googletest/primer.html

#include "TestParticleContainer.h"

TestParticleContainer::TestParticleContainer() {
  container = std::make_unique<ParticleContainer>(particles);

  for (int i = 0; i < NUM_PARTICLES; i++) {
    std::array<double, 3> v = {static_cast<double>(i * 3 % 7), static_cast<double>(i * 5 % 8) / 3,
                               static_cast<double>((i + 12) * 3)};
    std::array<double, 3> x = {static_cast<double>(i * 3 % 7), static_cast<double>(i % 3 + 4) / 2,
                               static_cast<double>((i + 81) * 12 % 41) * 2.4};
    double m = static_cast<double>(i);

    container->particles.emplace_back(x, v, m, i);
  }
}

/**
 * @test Linear iteration
 *
 * This test case checks if the container can be iterated through linearly in order
 *
 */
TEST_F(TestParticleContainer, IterationWorks) {
  int i = 0;
  for (Particle &particle : container->particles) {
    EXPECT_EQ(particle.getType(), i);

    if (i++ >= NUM_PARTICLES) {
      FAIL();
    }
  }
}

/**
 * @test PairIteration
 *
 * This test case checks wether the pair iteration works.
 * For a given vector, it should iterate over all unique pairs,
 * e.g
 * ```
 * (0, 1, 2, 3) -> [0, 1] [0, 2] [0, 3] [1, 2] [1, 3] [2, 3]
 * ```
 */
TEST_F(TestParticleContainer, PairIterationWorks) {
  constexpr unsigned int n = TestParticleContainer::NUM_PARTICLES;
  constexpr unsigned int N = (n * (n - 1)) / 2;

  std::vector<std::pair<int, int>> arr;
  arr.reserve(N);
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      arr.emplace_back(i, j);
    }
  }

  int i = 0;
  for (auto [p1, p2] : container->pairs()) {
    EXPECT_EQ(p1.getType(), arr[i].first);
    EXPECT_EQ(p2.getType(), arr[i].second);

    if (i++ >= N) {
      FAIL();
    }
  }
}
