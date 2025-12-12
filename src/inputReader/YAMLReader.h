#pragma once

#include <filesystem>
#include <vector>

#include "Particle.h"
#include "Settings.h"

class Settings;

/**
 * @class YAMLReader
 * @brief Parse particles and simulation parameters from yaml files
 */
class YAMLReader {
 public:
  static void readFile(std::vector<Particle> &particles, const std::filesystem::path filepath, Settings &settings) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      spdlog::error("Error opening {}", filepath.string());
      exit(-1);
    }

    YAMLReader::parse(particles, file, settings);
  }

  /**
   * @brief Parse a YAML file into `particles` and `settings`
   *
   * @param particles Where to store the read particles
   * @param file Path to the yaml file
   * @param settings Where to store settings
   */
  static void parse(std::vector<Particle> &particles, std::istream &file, Settings &settings);
};
