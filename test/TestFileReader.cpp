#include "gtest/gtest.h"
#include "inputReader/CuboidReader.h"
#include "inputReader/FileReader.h"
#include "inputReader/XYZReader.h"

/**
 * @test Read one particle from file
 *
 * This tests if the XYZReader can parse a single line containing one particle
 */
TEST(Reader, XYZ) {
  std::vector<Particle> particles;

  const Vector3 p = {34.75, 0.26, -14.2e-4};
  const Vector3 v = {0.0, 0.0296, -3.0};
  const double mass = 4.776e-14;

  std::stringstream mockInput;
  mockInput << p[0] << " " << p[1] << " " << p[2] << "    ";
  mockInput << v[0] << " " << v[1] << " " << v[2] << "    ";
  mockInput << mass << std::endl;

  XYZReader::parse(particles, mockInput);

  EXPECT_DOUBLE_EQ(p[0], particles[0].getX()[0]);
  EXPECT_DOUBLE_EQ(p[1], particles[1].getX()[1]);
  EXPECT_DOUBLE_EQ(p[2], particles[2].getX()[2]);

  EXPECT_DOUBLE_EQ(v[0], particles[0].getV()[0]);
  EXPECT_DOUBLE_EQ(v[1], particles[1].getV()[1]);
  EXPECT_DOUBLE_EQ(v[2], particles[2].getV()[2]);

  EXPECT_DOUBLE_EQ(mass, particles[0].getM());
}
