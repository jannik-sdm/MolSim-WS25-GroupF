/**
 * @file MolSim.cpp
 *
 */
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <memory>

#include "Settings.h"
#include "container/directSum/ParticleContainer.h"
#include "container/linkedCells/LinkedCells.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "outputWriter/YAMLWriter.h"
#include "simulations/CollisionSimulation.h"
#include "simulations/CutoffSimulation.h"
#include "simulations/PlanetSimulation.h"
#include "simulations/ThermostatSimulation.h"

/**
 * @brief plot the particles to a xyz-file or to a vtk-file.
 *
 * If ENABLE_VTK_OUTPUT is set, this function creates a vtk-file. Otherwise it creates a xyz-file
 */
void plotParticles(std::vector<Particle> &particles, int iteration, std::filesystem::path filename);

int main(int argc, char *argsv[]) {
  initializeLogging();

  std::vector<Particle> input_particles;
  Settings settings = Settings(input_particles);
  settings.parseArguments(argc, argsv);
  spdlog::set_level(settings.output.log_level);

  if (input_particles.empty()) {
    spdlog::warn("No particles to simulate");
    exit(EXIT_SUCCESS);
  }

  if (!settings.simulation.worksheet.has_value()) {
    spdlog::warn("No simulation selected");
    exit(EXIT_SUCCESS);
  }

  if (!settings.simulation.end_time.has_value()) {
    spdlog::error("Missing value for end_time");
    exit(EXIT_SUCCESS);
  }
  if (!settings.simulation.delta_t.has_value()) {
    spdlog::error("Missing value for delta_t");
    exit(EXIT_SUCCESS);
  }

  // select simulation
  std::unique_ptr<Simulation> simulation = nullptr;
  std::unique_ptr<Thermostat> thermostat = nullptr;

#ifdef ENABLE_TIME_MEASURE
  std::chrono::milliseconds total_runtime(0);
  std::vector<Particle> original = input_particles;  // keep a copy of the starting particles

  for (int i = 0; i < ENABLE_TIME_MEASURE; i++) {
    spdlog::info("Benchmark iteration {}/{}", i + 1, ENABLE_TIME_MEASURE);
#endif

    switch (settings.simulation.worksheet.value()) {
      case 1:
        simulation = std::make_unique<PlanetSimulation>(input_particles, settings.simulation.start_time,
                                                        settings.simulation.end_time.value(),
                                                        settings.simulation.delta_t.value());
        break;

      case 2:
        simulation = std::make_unique<CollisionSimulation>(input_particles, settings.simulation.start_time,
                                                           settings.simulation.end_time.value(),
                                                           settings.simulation.delta_t.value());
        break;

      case 3:
        simulation = std::make_unique<CutoffSimulation>(
            input_particles, settings.simulation.start_time, settings.simulation.end_time.value(),
            settings.simulation.delta_t.value(), settings.simulation.domain.value(),
            settings.simulation.cutoff_radius.value(), settings.simulation.borders.value(), settings.simulation.is2D,
            settings.simulation.gravity.value_or(0.0));

        /*(std::vector<Particle> &particles, const double start_time, const double end_time,
                   const double delta_t, const Vector3 &dimension, const double cutoff_radius,
                   const std::array<BorderType, 6> &border, const bool is2D, double g_grav)*/
        break;
      case 4: {
        thermostat = std::make_unique<Thermostat>(
            input_particles, settings.simulation.is2D, settings.simulation.t_frequency.value(),
            settings.simulation.t_final.value(), settings.simulation.t_max_change.value(),
            settings.simulation.t_initial.value(), settings.simulation.brown_motion_avg_velocity.value_or(0.0));
        simulation = std::make_unique<ThermostatSimulation>(
            input_particles, settings.simulation.start_time, settings.simulation.end_time.value(),
            settings.simulation.delta_t.value(), settings.simulation.domain.value(),
            settings.simulation.cutoff_radius.value(), settings.simulation.borders.value(), settings.simulation.is2D,
            settings.simulation.gravity.value_or(0.0), *thermostat);
      } break;

      default:
        spdlog::error("Invalid worksheet number {}", settings.simulation.worksheet.value());
        exit(EXIT_FAILURE);
    };

    // Disable logging
    // spdlog::set_level(spdlog::level::off);

    // Source for duration measurement- https://stackoverflow.com/a/19312610
    auto start_time_iteration = std::chrono::high_resolution_clock::now();

#ifdef ENABLE_TIME_MEASURE
    simulation->run([](const unsigned int _) {});
    // spdlog::set_level(settings.output.log_level);
#else
  if (settings.output.directory.has_value()) {
    spdlog::info("Writing files to {}", settings.output.directory.value().string());
    simulation->run([&input_particles, &settings](const unsigned int iteration) {
      if (iteration % settings.output.frequency == 0) {
        const auto filename = settings.output.directory.value() / settings.output.prefix;
        plotParticles(input_particles, static_cast<int>(iteration), filename);
      }
    });
  } else {
    spdlog::warn("No output folder set, running simulation without plotting");
    simulation->run([](const unsigned int _) {});
  }
#endif

    auto end_time_iteration = std::chrono::high_resolution_clock::now();
    spdlog::info(
        "Program has been running for {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time_iteration - start_time_iteration).count());

#ifdef ENABLE_TIME_MEASURE
    // copy original particles to start simulation from scratch
    input_particles.clear();
    for (const auto &p : original) {
      input_particles.push_back(p);
    }

    // add to total runtime
    total_runtime += std::chrono::duration_cast<std::chrono::milliseconds>(end_time_iteration - start_time_iteration);
  }

  // molecule updates per second = particles times iterations per second
  double mups = input_particles.size() * settings.simulation.end_time.value() / settings.simulation.delta_t.value();
  std::chrono::milliseconds average_runtime = total_runtime / ENABLE_TIME_MEASURE;
  spdlog::info("Benchmark finished: total={}ms, average={}ms, mups={:.0f}mol/s", total_runtime.count(),
               average_runtime.count(), mups);

#endif

  if (settings.output.export_filename.has_value())
    outputWriter::exportYAML(input_particles, settings, settings.output.export_filename.value());
  return 0;
}

void plotParticles(std::vector<Particle> &particles, int iteration, std::filesystem::path filename) {
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particles, filename.string(), iteration);
}
