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

/**
 * @brief plot the particles to a xyz-file or to a vtk-file.
 *
 * If ENABLE_VTK_OUTPUT is set, this function creates a vtk-file. Otherwise it creates a xyz-file
 */
void plotParticles(std::vector<Particle> &particles, int iteration, std::filesystem::path outputFolder);

int main(int argc, char *argsv[]) {
  initializeLogging();

  std::vector<Particle> input_particles;
  Settings settings = Settings(input_particles);
  settings.parseArguments(argc, argsv);

  if (input_particles.empty()) {
    spdlog::warn("No particles to simulate");
    exit(EXIT_SUCCESS);
  }

  if (!settings.simulation.worksheet.has_value()) {
    spdlog::warn("No simulation selected");
    exit(EXIT_SUCCESS);
  }

  if (!settings.simulation.delta_t.has_value()) {
    spdlog::error("Missing value for delta_t");
    exit(EXIT_SUCCESS);
  }

#ifdef ENABLE_TIME_MEASURE
  auto total_start_time_measure = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < ENABLE_TIME_MEASURE; i++) {
#endif

    // Source for duration measurement- https://stackoverflow.com/a/19312610
    auto start_time_measure = std::chrono::high_resolution_clock::now();

    // select simulation
    std::unique_ptr<Simulation> simulation = nullptr;

    switch (settings.simulation.worksheet.value()) {
      case 1:
        simulation = std::make_unique<PlanetSimulation>(input_particles, settings.simulation.end_time.value(),
                                                        settings.simulation.delta_t.value());
        break;

      case 2:
        simulation = std::make_unique<CollisionSimulation>(input_particles, settings.simulation.end_time.value(),
                                                           settings.simulation.delta_t.value());
        break;

      case 3:
        simulation = std::make_unique<CutoffSimulation>(
            input_particles, settings.simulation.domain.value(), settings.simulation.end_time.value(),
            settings.simulation.delta_t.value(), settings.simulation.cutoff_radius.value(),
            settings.simulation.borders.value(), settings.simulation.is2D);
        break;
      default:
        spdlog::error("Invalid worksheet number {}", settings.simulation.worksheet.value());
        exit(EXIT_FAILURE);
    };

    double current_time = settings.simulation.start_time;
    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < settings.simulation.end_time.value_or(settings.simulation.start_time)) {
      simulation->iteration();
      iteration++;

      if (iteration % settings.output.frequency == 0) {
        plotParticles(input_particles, iteration, settings.output.directory);
      }
      spdlog::info("Iteration {} finished.", iteration);

      current_time += settings.simulation.delta_t.value();
    }

    spdlog::info("output written. Terminating...");
    auto end_time_measure = std::chrono::high_resolution_clock::now();
    spdlog::info("Program has been running for {} ms",
                 std::chrono::duration_cast<std::chrono::milliseconds>(end_time_measure - start_time_measure).count());

#ifdef ENABLE_TIME_MEASURE
  }

  auto total_end_time_measure = std::chrono::high_resolution_clock::now();
  if (spdlog::get_level() <= spdlog::level::info) {
    spdlog::info("Total runtime: {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(total_end_time_measure -
                                                                                              total_start_time_measure)
                                            .count());
  } else
    std::cout << "Total Runtime: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(total_end_time_measure -
                                                                       total_start_time_measure)
                     .count()
              << "ms.\n";

#endif

  if (settings.output.export_filename.has_value())
    outputWriter::exportYAML(input_particles, settings, settings.output.export_filename.value());
  return 0;
}

void plotParticles(std::vector<Particle> &particles, int iteration, std::filesystem::path outputFolder) {
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particles, outputFolder, iteration);
}
