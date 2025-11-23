/**
 * @file MolSim.cpp
 *
 */
#include <chrono>
#include <iostream>

#include "ParticleContainer.h"
#include "Settings.h"
#include "outputWriter/VTKWriter.h"
#include "outputWriter/XYZWriter.h"
#include "simulations/CollisionSimulation.h"
#include "simulations/PlanetSimulation.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks-inl.h"
#include "spdlog/spdlog.h"

/**
 * @brief plot the particles to a xyz-file or to a vtk-file.
 *
 * If ENABLE_VTK_OUTPUT is set, this function creates a vtk-file. Otherwise it creates a xyz-file
 */
void plotParticles(int iteration, std::filesystem::path outputFolder);

/**
 * @brief Initialize spdlog
 *
 * Sets some default options and enables async logging for spdlog
 * @see https://github.com/gabime/spdlog
 */
void initializeLogging() {
  spdlog::init_thread_pool(8192, 1);
  // Create Sinks
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/log.txt", true /*überschreibt File, falls schon existend*/);
  // Create Logger
  std::vector<spdlog::sink_ptr> sinks{stdout_sink, file_sink};
  auto async_logger = std::make_shared<spdlog::async_logger>(
      "async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
  // Set Defaults
  spdlog::set_default_logger(async_logger);

  spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
}

ParticleContainer particleContainer;

int main(int argc, char *argsv[]) {
  initializeLogging();

  Settings settings = Settings(argc, argsv, particleContainer.particles);

  if (settings.isHelp()) {
    Settings::printHelp();
    exit(EXIT_SUCCESS);
  } else if (settings.isError()) {
    Settings::printHelp();
    exit(EXIT_FAILURE);
  }

  if (particleContainer.particles.empty()) {
    spdlog::warn("No particles to simulate");
    exit(EXIT_SUCCESS);
  }

  // use given parameters, or default endtime = 1000 delta_t = 0.014
  spdlog::info("Parsed Arguments:");
  spdlog::info("Starting simulation with parameters:");
  spdlog::info("endtime = {}", settings.end_time);
  spdlog::info("delta_t = {}", settings.delta_t);
  spdlog::info("brown_motion_mean = {}", settings.brown_motion_avg_velocity);
  spdlog::info("output path/name = {}", settings.outputFolder.string());

#ifdef ENABLE_TIME_MEASURE
  auto total_start_time_measure = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < ENABLE_TIME_MEASURE; i++) {
#endif

    // Source for duration measurement- https://stackoverflow.com/a/19312610
    auto start_time_measure = std::chrono::high_resolution_clock::now();

    // select simulation
    std::unique_ptr<Simulation> simulation = nullptr;

    switch (settings.worksheet) {
      case 1:
        simulation = std::make_unique<PlanetSimulation>(particleContainer, settings.end_time, settings.delta_t);
        break;

      case 2:
      default:
        simulation = std::make_unique<CollisionSimulation>(particleContainer, settings.end_time, settings.delta_t);
        break;
    };

    double current_time = settings.start_time;
    int iteration = 0;

    // for this loop, we assume: current x, current f and current v are known
    while (current_time < settings.end_time) {
      simulation->iteration();
      iteration++;

      if (iteration % settings.frequency == 0) {
        plotParticles(iteration, settings.outputFolder);
      }
      spdlog::info("Iteration {} finished.", iteration);

      current_time += settings.delta_t;
    }

    spdlog::info("output written. Terminating...");
    auto end_time_measure = std::chrono::high_resolution_clock::now();
    spdlog::info("Program has been running for {} ms",
                 std::chrono::duration_cast<std::chrono::milliseconds>(end_time_measure - start_time_measure).count());

#ifdef ENABLE_TIME_MEASURE
  }

  auto total_end_time_measure = std::chrono::high_resolution_clock::now();
  spdlog::info(
      "Total runtime: {}ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(total_end_time_measure - total_start_time_measure).count());
#endif

  return 0;
}

void plotParticles(int iteration, std::filesystem::path outputFolder) {
#ifdef ENABLE_VTK_OUTPUT
  outputWriter::VTKWriter writer;
#else
  outputWriter::XYZWriter writer;
#endif
  writer.plotParticles(particleContainer.particles, outputFolder, iteration);
}
