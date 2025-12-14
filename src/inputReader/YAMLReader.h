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

  static std::array<BorderType, 6> string_to_border_type(std::array<std::string, 6> border_strings) {
    std::array<BorderType, 6> result;
    const std::unordered_map<std::string, BorderType> lookup = {
        {"outflow", OUTFLOW},
        {"reflection", REFLECTION},
        {"periodic", PERIODIC},
        {"naive_reflection", NAIVE_REFLECTION},
    };

    for (size_t i = 0; i < border_strings.size(); i++) {
      auto border_type = lookup.find(border_strings[i]);
      if (border_type == lookup.end()) {
        spdlog::error("Border Type {} does not exist", border_strings[i]);
        result[i] = OUTFLOW;
      }

      result[i] = border_type->second;
    }

    return result;
  }
};
