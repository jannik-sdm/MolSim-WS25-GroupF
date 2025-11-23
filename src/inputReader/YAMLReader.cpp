#include "YAMLReader.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "../ParticleGenerator.h"
#include "yaml-cpp/yaml.h"

void YAMLReader::parse(std::vector<Particle> &particles, std::filesystem::path file, Settings &settings) {
  YAML::Node config = YAML::LoadFile(file.string());

  YAML::Node output = config["output"];
  if (output["folder"]) settings.outputFolder = output["folder"].as<std::string>();
  if (output["frequency"]) settings.frequency = output["frequency"].as<unsigned int>();

  YAML::Node simulation = config["simulation"];
  if (simulation["end_time"]) settings.end_time = simulation["end_time"].as<double>();
  if (simulation["delta_t"]) settings.delta_t = simulation["delta_t"].as<double>();
  if (simulation["brown_motion_avg_velocity"])
    settings.brown_motion_avg_velocity = simulation["brown_motion_avg_velocity"].as<double>();
  if (simulation["cutoff_radius"]) settings.cutoff_radius = simulation["cutoff_radius"].as<double>();
  if (simulation["domain"]) settings.domain = simulation["domain"].as<Vector3>();

  YAML::Node parts = config["particles"];
  for (auto p : parts) {
    if (p["cuboid"]) {
      YAML::Node cuboid = p["cuboid"];

      Vector3 x = cuboid["position"].as<Vector3>();
      std::array<unsigned int, 3> n = cuboid["size"].as<std::array<unsigned int, 3>>();
      double distance = cuboid["distance"].as<double>();
      double mass = cuboid["mass"].as<double>();
      Vector3 v = cuboid["velocity"].as<Vector3>();
      spdlog::info("generating cuboid: {}, {}, {}", x[0], x[1], x[2]);
      spdlog::info("-> {}, {}, {}", n[0], n[1], n[2]);
      spdlog::info("-> {}, {}", distance, mass);
      spdlog::info("-> {}, {}, {}", v[0], v[1], v[2]);

      ParticleGenerator::cuboid(particles, x, n, distance, mass, v);
    } else if (p["single"]) {
      YAML::Node single = p["single"];
      Vector3 x = single["position"].as<Vector3>();
      Vector3 v = single["velocity"].as<Vector3>();
      double mass = single["mass"].as<double>();

      particles.emplace_back(x, v, mass);
    }
  }
}
