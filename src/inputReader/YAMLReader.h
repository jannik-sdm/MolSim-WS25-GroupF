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
  static void parse(std::vector<Particle> &particles, std::filesystem::path file, Settings &settings);
};
