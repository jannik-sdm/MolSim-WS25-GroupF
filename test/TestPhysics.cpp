/**
 * @file TestPhysics.cpp
 *
 * Contains tests for the physics calculations
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <random>

#include "Particle.h"
#include "simulations/Physics.h"
#include "utils/ArrayUtils.h"

/**
 * @brief Macro to compare Vectors
 *
 * @see Particle Vector3 is defined as type alias here
 */
#define ASSERT_VECTOR3_EQ(expected, actual)   \
  do {                                        \
    ASSERT_DOUBLE_EQ(expected[0], actual[0]); \
    ASSERT_DOUBLE_EQ(expected[1], actual[1]); \
    ASSERT_DOUBLE_EQ(expected[2], actual[2]); \
  } while (false);

/**
 * @brief Timestep used for testing physics
 *
 * Using 1 to make manual calculations simpler
 */
const double delta_t = 1.0;

/* ========== Position Tests ========== */
/**
 * @test New Position, no velocity, no force
 *
 * When a particle has no velocity or force, it should not move.
 * This test checks this rule for a particle positioned at (0, 0, 0)
 * and a particle at a random position
 */
TEST(Position, NoVelocityNoForce) {
  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);

  Vector3 x = Physics::calculateX(p, delta_t);

  ASSERT_VECTOR3_EQ(p.getX(), x);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 position;
  std::generate(position.begin(), position.end(), [&]() { return dis(gen); });

  auto p2 = Particle(position, {0, 0, 0}, 1);

  Vector3 x2 = Physics::calculateX(p2, delta_t);

  ASSERT_VECTOR3_EQ(p2.getX(), x2);
}

/**
 * @test New position, fixed velocity, no force
 *
 * When a particle has a velocity but no force, the particle should move exactly $v * Δt$.
 * This test checks this rule for a particle positioned at (0, 0, 0)
 * and a particle at a random position
 */
TEST(Position, FixedVelocityNoForce) {
  const Vector3 velocity = {1, 1, 1};
  auto p = Particle({0, 0, 0}, velocity, 1);

  Vector3 x = Physics::calculateX(p, delta_t);
  const Vector3 difference = delta_t * velocity;

  ASSERT_VECTOR3_EQ((p.getX() + difference), x);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 position;
  std::generate(position.begin(), position.end(), [&]() { return dis(gen); });

  auto p2 = Particle(position, velocity, 1);

  Vector3 x2 = Physics::calculateX(p2, delta_t);

  ASSERT_VECTOR3_EQ((p2.getX() + difference), x2);
}

/**
 * @test New position, random velocity, no force
 *
 * When a particle has a velocity but no force, the particle should move exactly $v * Δt$.
 * This test checks this rule for a particle positioned at (0, 0, 0)
 * and a particle at a random position
 */
TEST(Position, RandomVelocityNoForce) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 velocity;
  std::generate(velocity.begin(), velocity.end(), [&]() { return dis(gen); });

  auto p = Particle({0, 0, 0}, velocity, 1);

  Vector3 x = Physics::calculateX(p, delta_t);
  const Vector3 difference = delta_t * velocity;

  ASSERT_VECTOR3_EQ((p.getX() + difference), x);

  Vector3 position;
  std::generate(position.begin(), position.end(), [&]() { return dis(gen); });

  auto p2 = Particle(position, velocity, 1);

  Vector3 x2 = Physics::calculateX(p2, delta_t);

  ASSERT_VECTOR3_EQ((p2.getX() + difference), x2);
}

/**
 * @test New position, no velocity, fixed force
 *
 * When a particle has no velocity but a force, the particle should move exactly $Δt² * F/2m$.
 * This test checks this rule for a particle positioned at (0, 0, 0)
 * and a particle at a random position
 */
TEST(Position, NoVelocityFixedForce) {
  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);
  p.setF({2, 2, 2});

  Vector3 x = Physics::calculateX(p, delta_t);
  Vector3 difference = {delta_t, delta_t, delta_t};

  ASSERT_VECTOR3_EQ((p.getX() + difference), x);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 position;
  std::generate(position.begin(), position.end(), [&]() { return dis(gen); });

  auto p2 = Particle(position, {0, 0, 0}, 1);
  p2.setF({2, 2, 2});

  Vector3 x2 = Physics::calculateX(p2, delta_t);

  ASSERT_VECTOR3_EQ((p2.getX() + difference), x2);
}

/**
 * @test New position, random velocity, no force
 *
 * When a particle has a velocity but no force, the particle should move exactly $v * Δt$.
 * This test checks this rule for a particle positioned at (0, 0, 0)
 * and a particle at a random position
 */
TEST(Position, NoVelocityRandomForce) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 force;
  std::generate(force.begin(), force.end(), [&]() { return dis(gen); });

  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);
  p.setF(force);

  Vector3 x = Physics::calculateX(p, delta_t);
  const Vector3 difference = (delta_t / 2) * force;

  ASSERT_VECTOR3_EQ((p.getX() + difference), x);

  Vector3 position;
  std::generate(position.begin(), position.end(), [&]() { return dis(gen); });

  auto p2 = Particle(position, {0, 0, 0}, 1);
  p2.setF(force);

  Vector3 x2 = Physics::calculateX(p2, delta_t);

  ASSERT_VECTOR3_EQ((p2.getX() + difference), x2);
}

/* ========== Velocity Tests ========== */
/**
 * @test New velocity, no force
 *
 * When a particle has no force, the new velocity should be the same as the previous one
 * This test checks this for a particle with velocity (0, 0, 0)
 * and a particle with a random velocity
 */
TEST(Velocity, NoForce) {
  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);
  Vector3 v = Physics::calculateV(p, delta_t);

  ASSERT_VECTOR3_EQ(p.getV(), v);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 velocity;
  std::generate(velocity.begin(), velocity.end(), [&]() { return dis(gen); });

  p.setV(velocity);

  Vector3 v2 = Physics::calculateV(p, delta_t);

  ASSERT_VECTOR3_EQ(p.getV(), v2);
}

/**
 * @test New velocity, fixed force
 *
 * When a particle has a fixed force for both timesteps, the new velocity should be $Δt * 2F/2m$
 * This test checks this for a particle with velocity (0, 0, 0)
 * and a particle with a random velocity
 */
TEST(Velocity, FixedForce) {
  Vector3 force = {0, 0, 0};
  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);
  p.setF(force);
  p.setF(force);  // call twice so that old_f is equal to new_f

  Vector3 v = Physics::calculateV(p, delta_t);
  Vector3 difference = force;

  ASSERT_VECTOR3_EQ((p.getV() + difference), v);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 velocity;
  std::generate(velocity.begin(), velocity.end(), [&]() { return dis(gen); });

  p.setV(velocity);

  Vector3 v2 = Physics::calculateV(p, delta_t);

  ASSERT_VECTOR3_EQ((p.getV() + difference), v2);
}

/**
 * @test New velocity, random force
 *
 * When a particle has a random force, the new velocity should be $Δt * F + F/2m$
 * This test checks this for a particle with velocity (0, 0, 0)
 * and a particle with a random velocity
 */
TEST(Velocity, RandomForce) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  Vector3 force;
  std::generate(force.begin(), force.end(), [&]() { return dis(gen); });

  auto p = Particle({0, 0, 0}, {0, 0, 0}, 1);
  p.setF(force);
  p.setF(force);  // call twice so that old_f is equal to new_f

  Vector3 v = Physics::calculateV(p, delta_t);
  Vector3 difference = force;

  ASSERT_VECTOR3_EQ((p.getV() + difference), v);

  Vector3 velocity;
  std::generate(velocity.begin(), velocity.end(), [&]() { return dis(gen); });

  p.setV(velocity);

  Vector3 v2 = Physics::calculateV(p, delta_t);

  ASSERT_VECTOR3_EQ((p.getV() + difference), v2);
}

/* ========== Planet Force Tests ========== */
TEST(PlanetForce, FixedMass) {
  auto p1 = Particle({2, 0, 0}, {0, 0, 0}, 1);
  auto p2 = Particle({1, 0, 0}, {0, 0, 0}, 1);

  Vector3 f = Physics::planetForce(p1, p2);
  Vector3 expected = p2.getX() - p1.getX();

  ASSERT_VECTOR3_EQ(expected, f);
}

TEST(PlanetForce, RandomMass) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-100.0, 100.0);

  double m1 = dis(gen);
  double m2 = dis(gen);

  auto p1 = Particle({2, 0, 0}, {0, 0, 0}, m1);
  auto p2 = Particle({1, 0, 0}, {0, 0, 0}, m2);

  Vector3 f = Physics::planetForce(p1, p2);
  Vector3 expected = m1 * m2 * (p2.getX() - p1.getX());

  ASSERT_VECTOR3_EQ(expected, f);
}

/* ========== Lennard Jones Force Tests ========== */
TEST(LennardJonesForce, FixedMass) {
  auto p1 = Particle({2, 0, 0}, {0, 0, 0}, 1);
  auto p2 = Particle({1, 0, 0}, {0, 0, 0}, 1);

  Vector3 f = Physics::lennardJonesForce(p1, p2, 1, 1);
  Vector3 expected = 24 * (p1.getX() - p2.getX());

  ASSERT_VECTOR3_EQ(expected, f);
}
