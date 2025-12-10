#include "YAMLReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "../LinkedCells/Cell.h"
#include "../ParticleGenerator.h"
#include "../utils/ArrayUtils.h"

void YAMLReader::parse(std::vector<Particle> &particles, std::istream &file, Settings &settings) {
  YAML::Node config = YAML::Load(file);

  YAML::Node output = config["output"];
  if (output["folder"]) settings.outputFolder = output["folder"].as<std::string>();
  if (output["frequency"]) settings.frequency = output["frequency"].as<unsigned int>();

  YAML::Node simulation = config["simulation"];
  if (simulation["worksheet"]) settings.worksheet = simulation["worksheet"].as<unsigned int>();
  if (simulation["end_time"]) settings.end_time = simulation["end_time"].as<double>();
  if (simulation["delta_t"]) settings.delta_t = simulation["delta_t"].as<double>();
  if (simulation["brown_motion_avg_velocity"])
    settings.brown_motion_avg_velocity = simulation["brown_motion_avg_velocity"].as<double>();
  if (simulation["cutoff_radius"]) settings.cutoff_radius = simulation["cutoff_radius"].as<double>();

  if (simulation["domain"]) settings.domain = simulation["domain"].as<Vector3>();
  if (simulation["borders"]) {
    std::array<std::string, 6> borderStrings = simulation["borders"].as<std::array<std::string, 6>>();
    std::array<BorderType, 6> &borders = settings.borders;
    for (int i = 0; i < 6; i++) {
      auto &borderString = borderStrings[i];
      if (borderString == "reflection")
        borders[i] = REFLECTION;
      else if (borderString == "periodic")
        borders[i] = PERIODIC;
      else if (borderString == "naive Reflection")
        borders[i] = NAIVE_REFLECTION;
      else
        borders[i] = OUTFLOW;
    }
  }
  if (simulation["dimension"]) settings.is2D = simulation["dimension"].as<std::string>() == "2D";

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

      spdlog::debug("Generating single particle: position={} mass={} velocity={}", ArrayUtils::to_string(x), mass,
                    ArrayUtils::to_string(v));
      particles.emplace_back(x, v, mass);
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
