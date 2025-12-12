#include "TestYAMLReader.h"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>

#include "inputReader/YAMLReader.h"
#include "utils/ArrayUtils.h"

/**
 * @test Output folder
 *
 * Tests if the parser correctly reads output.folder
 */
TEST_F(TestYAMLReader, OutputFolder) {
  const std::string folder = "out/";
  std::stringstream input;
  input << "output:\n  folder: " << '"' << folder << '"' << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_EQ(settings.outputFolder, folder);
}

/**
 * @test Output frequency
 *
 * Tests if the parser correctly reads output.frequency
 */
TEST_F(TestYAMLReader, OutputFrequency) {
  const unsigned int frequency = 10;
  std::stringstream input;
  input << "output:\n  frequency: " << frequency << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_EQ(settings.frequency, frequency);

  const int bad_frequency = -10;
  std::stringstream bad_input;
  bad_input << "output:\n  frequency: " << bad_frequency << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);
}

/**
 * @test simulation worksheet
 *
 * Tests if the parser correctly reads simulation.worksheet
 */
TEST_F(TestYAMLReader, SimulationWorksheet) {
  const int bad_worksheet = -1;
  std::stringstream bad_input;
  bad_input << "simulation:\n  worksheet: " << bad_worksheet << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const unsigned int worksheet = 1;
  std::stringstream input;
  input << "simulation:\n  worksheet: " << worksheet << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.worksheet.has_value());
  EXPECT_EQ(settings.worksheet.value(), worksheet);
}

/**
 * @test simulation delta_t
 *
 * Tests if the parser correctly reads simulation.delta_t
 */
TEST_F(TestYAMLReader, SimulationDeltat) {
  const std::string bad_delta_t = "str";
  std::stringstream bad_input;
  bad_input << "simulation:\n  delta_t: " << '"' << bad_delta_t << '"' << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const double delta_t = 0.0005;
  std::stringstream input;
  input << "simulation:\n  delta_t: " << delta_t << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.delta_t.has_value());
  EXPECT_DOUBLE_EQ(settings.delta_t.value(), delta_t);
}

/**
 * @test simulation end_time
 *
 * Tests if the parser correctly reads simulation.end_time
 */
TEST_F(TestYAMLReader, SimulationEndTime) {
  const std::string bad_end_time = "str";
  std::stringstream bad_input;
  bad_input << "simulation:\n  end_time: " << '"' << bad_end_time << '"' << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const double end_time = 20.0;
  std::stringstream input;
  input << "simulation:\n  end_time: " << end_time << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.end_time.has_value());
  EXPECT_DOUBLE_EQ(settings.end_time.value(), end_time);
}

/**
 * @test simulation brown_motion_avg_velocity
 *
 * Tests if the parser correctly reads simulation.brown_motion_avg_velocity
 */
TEST_F(TestYAMLReader, SimulationBrownMotionAvgVelocity) {
  const std::string bad_brown_motion_avg_velocity = "str";
  std::stringstream bad_input;
  bad_input << "simulation:\n  brown_motion_avg_velocity: " << '"' << bad_brown_motion_avg_velocity << '"' << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const double brown_motion_avg_velocity = 0.1;
  std::stringstream input;
  input << "simulation:\n  brown_motion_avg_velocity: " << brown_motion_avg_velocity << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.brown_motion_avg_velocity.has_value());
  EXPECT_DOUBLE_EQ(settings.brown_motion_avg_velocity.value(), brown_motion_avg_velocity);
}

/**
 * @test simulation cutoff_radius
 *
 * Tests if the parser correctly reads simulation.cutoff_radius
 */
TEST_F(TestYAMLReader, SimulationCutoffRadius) {
  const std::string bad_cutoff_radius = "str";
  std::stringstream bad_input;
  bad_input << "simulation:\n  cutoff_radius: " << '"' << bad_cutoff_radius << '"' << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const double cutoff_radius = 3.0;
  std::stringstream input;
  input << "simulation:\n  cutoff_radius: " << cutoff_radius << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.cutoff_radius.has_value());
  EXPECT_DOUBLE_EQ(settings.cutoff_radius.value(), cutoff_radius);
}

/**
 * @test simulation domain
 *
 * Tests if the parser correctly reads simulation.domain
 */
TEST_F(TestYAMLReader, SimulationDomain) {
  const std::array<double, 2> bad_domain = {1, 1};
  std::stringstream bad_input;
  bad_input << "simulation:\n  domain: " << ArrayUtils::to_string(bad_domain) << std::endl;

  EXPECT_THROW(YAMLReader::parse(particles, bad_input, settings), YAML::BadConversion);

  const Vector3 domain = {1.5, 1.5, 1.5};
  std::stringstream input;
  input << "simulation:\n  domain: " << ArrayUtils::to_string(domain) << std::endl;

  YAMLReader::parse(particles, input, settings);

  EXPECT_TRUE(settings.domain.has_value());
  EXPECT_DOUBLE_EQ(settings.domain.value()[0], domain[0]);
  EXPECT_DOUBLE_EQ(settings.domain.value()[1], domain[1]);
  EXPECT_DOUBLE_EQ(settings.domain.value()[2], domain[2]);
}
