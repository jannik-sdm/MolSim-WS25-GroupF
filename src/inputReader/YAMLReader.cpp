#include "inputReader/YAMLReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "Settings.h"
#include "container/linkedCells/Cell.h"
#include "utils/ArrayUtils.h"
#include "utils/ParticleGenerator.h"

void YAMLReader::parse(std::vector<Particle> &particles, std::istream &file, Settings &settings) {
  YAML::Node config = YAML::Load(file);

  settings.output = config["output"].as<Settings::Output>();
  settings.simulation = config["simulation"].as<Settings::Simulation>();

  YAML::Node parts = config["particles"];
  for (auto p : parts) {
    if (p["cuboid"]) {
      YAML::Node cuboid = p["cuboid"];

      Vector3 x = cuboid["position"].as<Vector3>();
      std::array<unsigned int, 3> n = cuboid["size"].as<std::array<unsigned int, 3>>();
      double distance = cuboid["distance"].as<double>();
      double mass = cuboid["mass"].as<double>();
      Vector3 v = cuboid["velocity"].as<Vector3>();

      spdlog::debug("Generating cuboid: position={} size={} distance={} mass={} velocity={}", ArrayUtils::to_string(x),
                    ArrayUtils::to_string(n), distance, mass, ArrayUtils::to_string(v));

      ParticleGenerator::cuboid(particles, x, n, distance, mass, v);
    } else if (p["single"]) {
      YAML::Node single = p["single"];
      Vector3 x = single["position"].as<Vector3>();
      Vector3 v = single["velocity"].as<Vector3>();
      double mass = single["mass"].as<double>();

      if (single["force"]) {
        Vector3 f = single["force"].as<Vector3>();
        if (single["old_force"]) {
          Vector3 old_f = single["old_force"].as<Vector3>();
          spdlog::debug("Generating single particle: position={}, velocity={}, mass={}, force={}, old_force={}",
                        ArrayUtils::to_string(x), ArrayUtils::to_string(v), mass, ArrayUtils::to_string(f),
                        ArrayUtils::to_string(old_f));
          particles.emplace_back(x, v, mass, f, old_f);
        } else {
          spdlog::debug("Generating single particle: position={}, velocity={}, mass={}, force={}",
                        ArrayUtils::to_string(x), ArrayUtils::to_string(v), mass, ArrayUtils::to_string(f));
          particles.emplace_back(x, v, mass, f);
        }
      } else {
        spdlog::debug("Generating single particle: position={} mass={} velocity={}", ArrayUtils::to_string(x), mass,
                      ArrayUtils::to_string(v));
        particles.emplace_back(x, v, mass);
      }
    } else if (p["disc"]) {
      YAML::Node disc = p["disc"];
      Vector3 position = disc["position"].as<Vector3>();
      int radius = disc["radius"].as<int>();
      double distance = disc["distance"].as<double>();
      double mass = disc["mass"].as<double>();
      Vector3 velocity = disc["velocity"].as<Vector3>();

      spdlog::debug("Generating disc: position={} radius={} distance={} mass={} velocity={}",
                    ArrayUtils::to_string(position), radius, distance, mass, ArrayUtils::to_string(velocity));
      ParticleGenerator::disc(particles, position, radius, distance, mass, velocity);
    }
  }
}
