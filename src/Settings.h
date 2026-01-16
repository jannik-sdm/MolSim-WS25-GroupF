#pragma once

#include <getopt.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <optional>

#include "container/linkedCells/Cell.h"

/**
 * @class Settings
 * @brief Wrapper for parsing parameters and inputs from files or command line arguments
 *
 * @todo Parameters passed over the command line should overwrite parameters from files, even if the file is read later
 */
class Settings {
 public:
  struct Output {
    /** @brief Path to the directory to write output files to */
    std::optional<std::filesystem::path> directory;
    /** @brief Prefix for the filename */
    std::string prefix = "iter";
    /** @brief Frequency of files writen \f$(iteration mod frequency = 0)\f$. Set this to 0 to disable file output */
    unsigned int frequency = 10;

    /** @brief Path to the filename to export */
    std::optional<std::filesystem::path> export_filename;

    /** @brief Log level for console/file output */
    spdlog::level::level_enum log_level = spdlog::level::info;
  };
  struct Output output;

  struct Membrane {
    std::optional<double> r0;
    std::optional<double> k;
    std::optional<double> f_zUp;
    std::optional<double> sigma;
    std::vector<Particle *> upwardsParticles;
  };
  struct Membrane membrane;

  struct Simulation {
    /** @brief Which worksheet to run */
    std::optional<unsigned int> worksheet;

    /** @brief Time where the simulation starts */
    double start_time = 0;
    /** @brief time until the simulation runs */
    std::optional<double> end_time;
    /** @brief amount of time that passes each iteration */
    std::optional<double> delta_t;

    /** @brief Domain */
    std::optional<Vector3> domain;
    /** @brief Cutoff radius for linked cell algorithm */
    std::optional<double> cutoff_radius;
    /** @brief BorderTypes of all 6 sides of the domain"*/
    std::optional<std::array<BorderType, 6>> borders;
    /** @brief Specifies if the simmulation should be 2D or 3D"*/
    bool is2D = false;
    /** @brief Average brownian motion velocity to initialize the particles with */
    std::optional<double> brown_motion_avg_velocity;
    /** @brief Initial Temperatur*/
    std::optional<double> t_initial;
    /** @brief Destination value of the Temperature*/
    std::optional<double> t_final;
    /** @brief Maximum Change of temperature in one step*/
    std::optional<double> t_max_change;
    /** @brief Temperature update frequency. How many iterations should it take to re-check temperature*/
    std::optional<unsigned int> t_frequency;
    /** @brief acceleration factor of the gravity*/
    std::optional<double> gravity;
  };
  struct Simulation simulation;

  Settings(std::vector<Particle> &particles) : particles(particles) {};

  /**
   * @brief Parse command line arguments
   *
   * @param argc
   * @param argv
   */
  void parseArguments(int argc, char *argv[]);

  /**
   * @brief Prints Help Message to the Commandline
   * @see https://gist.github.com/ashwin/d88184923c7161d368a9
   */
  static void printHelp();

  /**
   * @brief Create a output directory
   *
   * Creates all necessary directories to make sure that the directory pointed to by `directory` is valid.
   * Should behave similar to `mkdir -p <directory>`
   *
   * @param directory path to the output directory
   */
  static void createOutputDirectory(const std::filesystem::path &directory);

 private:
  /** @brief Container to store parsed particles in */
  std::vector<Particle> &particles;
};

/**
 * @brief Initialize spdlog
 *
 * Sets some default options and enables async logging for spdlog
 * @see https://github.com/gabime/spdlog
 */
void initializeLogging();
