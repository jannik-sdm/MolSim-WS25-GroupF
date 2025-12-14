#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <vector>

#include "Particle.h"
#include "Settings.h"
#include "inputReader/YAMLReader.h"
#include "utils/yamlPatches.h"

namespace outputWriter {

void exportYAML(std::vector<Particle> &particles, Settings &settings, std::filesystem::path filepath) {
  YAML::Node node;
  node["output"] = settings.output;
  node["simulation"] = settings.simulation;

  YAML::Node sequence(YAML::NodeType::Sequence);
  for (auto &p : particles) {
    YAML::Node map = YAML::Node(YAML::NodeType::Map);
    map["single"] = p;
    sequence.push_back(map);
  }

  node["particles"] = sequence;

  YAML::Emitter out;
  out << node;

  std::ofstream file(filepath);
  if (!file.good()) {
    spdlog::error("Failed to open {}", filepath.string());
    return;
  }

  file << out.c_str();
}
}  // namespace outputWriter
