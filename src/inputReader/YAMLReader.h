#pragma once

#include <filesystem>
#include <vector>

#include "../Particle.h"
#include "../Settings.h"

class Settings;

/**
 * @class YAMLReader
 * @brief Parse particles and simulation parameters from yaml files
 */
class YAMLReader {
 public:
  /**
   * @brief Parse a YAML file into `particles` and `settings`
   *
   * @param particles Where to store the read particles
   * @param file Path to the yaml file
   * @param settings Where to store settings
   */
  static void parse(std::vector<Particle> &particles, std::filesystem::path file, Settings &settings);
};
