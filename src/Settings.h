#pragma once

#include <getopt.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <optional>

#include "container/linkedCells/Cell.h"
#include "inputReader/CuboidReader.h"
#include "inputReader/FileReader.h"
#include "inputReader/XYZReader.h"
#include "inputReader/YAMLReader.h"

/**
 * @class Settings
 * @brief Wrapper for parsing parameters and inputs from files or command line arguments
 *
 * @todo Parameters passed over the command line should overwrite parameters from files, even if the file is read later
 */
class Settings {
 public:
  /** @brief Path to folder to write output files to */
  std::filesystem::path outputFolder;
  /** @brief Frequency of files writen \f$(iteration mod frequency = 0)\f$ */
  unsigned int frequency = 10;

  /** @brief Time where the simulation starts */
  const double start_time = 0;
  /** @brief time until the simulation runs */
  std::optional<double> end_time;
  /** @brief amount of time that passes each iteration */
  std::optional<double> delta_t;
  /** @brief Average brownian motion velocity to initialize the particles with */
  std::optional<double> brown_motion_avg_velocity;
  /** @brief Cutoff radius for linked cell algorithm */
  std::optional<double> cutoff_radius;
  /** @brief BorderTypes of all 6 sides of the domain"*/
  std::optional<std::array<BorderType, 6>> borders;
  /** @brief Specifies if the simmulation should be 2D or 3D"*/
  bool is2D = false;
  /** @brief Domain */
  std::optional<Vector3> domain;

  /** @brief Log level for console/file output */
  spdlog::level::level_enum log_level = spdlog::level::info;

  /** @brief Which worksheet to run */
  std::optional<unsigned int> worksheet;

  Settings(std::vector<Particle> &particles) : particles(particles) {};

  /**
   * @brief Parses the Arguments
   * @param argc number of commandline arguments
   * @param argv[] commandline arguments
   * @param particles reference to a particle vector
   */
  Settings(int argc, char *argv[], std::vector<Particle> &particles);

  /**
   * @brief Prints Help Message to the Commandline
   * @see https://gist.github.com/ashwin/d88184923c7161d368a9
   */
  static void printHelp();

  /**
   * @brief Check if the user requested help screen
   * @return true if the user requestd help screen
   */
  bool isHelp() { return parse_result == HELP; }
  /**
   * @brief Check if there was an error during parsing
   * @return true if there was an error during argument parsing
   */
  bool isError() { return parse_result == ERROR; }

 private:
  /** @brief Container to store parsed particles in */
  std::vector<Particle> &particles;
  /** @brief Possible outcomes of parsing command line arguments */
  enum PARSE_RESULT { SUCCESS, ERROR, HELP };
  /**
   * @brief store the result of calling argparse, for later usage
   *
   * @see isHelp(), isError()
   */
  PARSE_RESULT parse_result;

  /**
   * @brief Parse
   *
   * @param argc Number of command line arguments
   * @param argv Array of command line arguments
   * @return PARSE_RESULT result of parsing arguments
   */
  PARSE_RESULT parseArguments(int argc, char *argv[]);

  /**
   * @brief Convert a string to a valid loglevel
   *
   * Converts strings such as "TRACE" or "DEBUG" to valid spdlog-loglevels.
   * Loglevels are matched case insensitive. If the string cannot be matched,
   * it returns the default log level
   *
   * @param string User submitted string
   * @return spdlog::level::level_enum
   */
  spdlog::level::level_enum stringToLogLevel(std::string string);

  /**
   * @brief Create a output directory
   *
   * Creates all necessary directories to make sure that the directory pointed to by `directory` is valid.
   * Should behave similar to `mkdir -p <directory>`
   *
   * @param directory path to the output directory
   */
  void createOutputDirectory(std::filesystem::path directory);
};

/**
 * @brief Initialize spdlog
 *
 * Sets some default options and enables async logging for spdlog
 * @see https://github.com/gabime/spdlog
 */
void initializeLogging();
