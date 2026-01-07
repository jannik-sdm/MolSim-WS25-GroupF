#include "TestThermostatSimulation.h"

#include <spdlog/spdlog.h>

/**
 * @brief Test for holding a temperature
 * Initial T = 20, Target T = 20.
 * Expectation: Velocity remains exactly the same.
 */
TEST_F(TestThermostatSimulation, HoldingTemperature) {
  target_temp = 20.0;
  thermostat_interval = 1;  // Apply every step for immediate effect

  // Create particle with exactly temperature = 20
  Vector3 v_init = getVelocityForTemp(target_temp);
  particles.emplace_back(Vector3{5, 5, 5}, v_init, 1.0, 0);

  InitSimulation();

  sim->iteration();

  double final_temp = thermostat->calculateCurrentTemperature(1);
  EXPECT_NEAR(final_temp, 20.0, 1e-5);

  EXPECT_NEAR(particles[0].getV()[0], v_init[0], 1e-5);
}

/**
 * @brief Test for heating the system
 * Initial T = 10, Target T = 20. Max Change = Infinite.
 * Expectation: System heats directly to 20.
 */
TEST_F(TestThermostatSimulation, HeatingUncapped) {
  double init_temp = 10.0;
  target_temp = 20.0;
  thermostat_interval = 1;

  particles.emplace_back(Vector3{5, 5, 5}, getVelocityForTemp(init_temp), 1.0, 0);

  InitSimulation();

  ASSERT_NEAR(thermostat->calculateCurrentTemperature(1), init_temp, 1e-5);

  sim->iteration();

  double final_temp = thermostat->calculateCurrentTemperature(1);
  EXPECT_NEAR(final_temp, target_temp, 1e-5) << "Thermostat should heat to target immediately if max_change is high";
}

/**
 * @brief Test for cooling the system (uncapped)
 * Initial T = 30, Target T = 10. Max Change = Infinite.
 * Expectation: System cools directly to 10.
 */
TEST_F(TestThermostatSimulation, CoolingUncapped) {
  double init_temp = 30.0;
  target_temp = 10.0;
  thermostat_interval = 1;

  particles.emplace_back(Vector3{5, 5, 5}, getVelocityForTemp(init_temp), 1.0, 0);

  InitSimulation();
  sim->iteration();

  double final_temp = thermostat->calculateCurrentTemperature(1);
  EXPECT_NEAR(final_temp, target_temp, 1e-5);
}

/**
 * @brief Test for heating with max change limit
 * Initial T = 10, Target T = 100. Max Change = 5.
 * Expectation: System heats to 15 (10 + 5), NOT 100.
 */
TEST_F(TestThermostatSimulation, HeatingCapped) {
  double init_temp = 10.0;
  target_temp = 100.0;
  max_temp_change = 5.0;
  thermostat_interval = 1;

  particles.emplace_back(Vector3{5, 5, 5}, getVelocityForTemp(init_temp), 1.0, 0);

  InitSimulation();
  sim->iteration();

  double final_temp = thermostat->calculateCurrentTemperature(1);

  double expected_temp = init_temp + max_temp_change;  // 15.0

  EXPECT_NEAR(final_temp, expected_temp, 1e-5) << "Thermostat should only heat by max_temp_change";
}

/**
 * @brief Test for cooling with max change limit
 * Initial T = 50, Target T = 10. Max Change = 5.
 * Expectation: System cools to 45 (50 - 5), NOT 10.
 */
TEST_F(TestThermostatSimulation, CoolingCapped) {
  double init_temp = 50.0;
  target_temp = 10.0;
  max_temp_change = 5.0;
  thermostat_interval = 1;

  particles.emplace_back(Vector3{5, 5, 5}, getVelocityForTemp(init_temp), 1.0, 0);

  InitSimulation();
  sim->iteration();

  double final_temp = thermostat->calculateCurrentTemperature(1);

  double expected_temp = init_temp - max_temp_change;  // 45.0

  EXPECT_NEAR(final_temp, expected_temp, 1e-5);
}

/**
 * @brief Test for the interval logic of the thermostat
 * Interval = 10. Run 5 steps -> No Change. Run 10 steps -> Change.
 */
TEST_F(TestThermostatSimulation, ApplicationInterval) {
  double init_temp = 10.0;
  target_temp = 20.0;
  thermostat_interval = 10;

  particles.emplace_back(Vector3{5, 5, 5}, getVelocityForTemp(init_temp), 1.0, 0);

  InitSimulation();

  for (int i = 0; i < 9; ++i) sim->iteration();

  // Should NOT have changed yet
  EXPECT_NEAR(thermostat->calculateCurrentTemperature(1), init_temp, 1e-5) << "Thermostat applied too early!";

  // Run 10th iteration
  sim->iteration();

  // Should HAVE changed now
  EXPECT_NEAR(thermostat->calculateCurrentTemperature(1), target_temp, 1e-5) << "Thermostat did not apply at step 10";
}