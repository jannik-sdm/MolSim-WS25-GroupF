#include "YAMLReader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

// #include "../../cmake-build-debug/_deps/googletest-src/googlemock/include/gmock/gmock-matchers.h"
#include "Settings.h"
#include "container/linkedCells/Cell.h"
#include "utils/ArrayUtils.h"
#include "utils/ParticleGenerator.h"

void YAMLReader::parse(std::vector<Particle> &particles, std::istream &file, Settings &settings) {
  YAML::Node config = YAML::Load(file);

  auto output = config["output"];
  if (output) settings.output = config["output"].as<Settings::Output>();
  auto simulation = config["simulation"];
  if (simulation) settings.simulation = config["simulation"].as<Settings::Simulation>();

  auto alternative = config["useAlternateParallelisation"];
  if (alternative) settings.useAlternateParallelisation = config["useAlternateParallelisation"].as<bool>();

  YAML::Node parts = config["particles"];
  for (auto p : parts) {
    if (p["cuboid"]) {
      YAML::Node cuboid = p["cuboid"];

      Vector3 x = cuboid["position"].as<Vector3>();
      std::array<unsigned int, 3> n = cuboid["size"].as<std::array<unsigned int, 3>>();
      double distance = cuboid["distance"].as<double>();
      double mass = cuboid["mass"].as<double>();
      Vector3 v = cuboid["velocity"].as<Vector3>();
      std::optional<double> epsilon;
      std::optional<double> sigma;
      if (cuboid["epsilon"]) epsilon = cuboid["epsilon"].as<double>();
      if (cuboid["sigma"]) sigma = cuboid["sigma"].as<double>();

      SPDLOG_DEBUG("Generating cuboid: position={} size={} distance={} mass={} velocity={} sigma={} epsilon={}",
                   ArrayUtils::to_string(x), ArrayUtils::to_string(n), distance, mass, ArrayUtils::to_string(v),
                   sigma.value_or(1), epsilon.value_or(5));

      ParticleGenerator::cuboid(particles, x, n, distance, mass, epsilon, sigma, v);
    } else if (p["membrane"]) {
      YAML::Node membrane = p["membrane"];

      Vector3 x = membrane["position"].as<Vector3>();
      std::array<unsigned int, 3> n = membrane["size"].as<std::array<unsigned int, 3>>();
      double distance = membrane["distance"].as<double>();
      double mass = membrane["mass"].as<double>();
      Vector3 v = membrane["velocity"].as<Vector3>();
      std::optional<double> epsilon;

      if (membrane["epsilon"]) epsilon = membrane["epsilon"].as<double>();
      if (membrane["sigma"]) settings.membrane.sigma = membrane["sigma"].as<double>();
      std::optional<std::vector<std::array<int, 2>>> upwardsParticlesIndexes =
          membrane["upwards_particles_indexes"].as<std::vector<std::array<int, 2>>>();
      settings.membrane.k = membrane["k"].as<double>();
      settings.membrane.r0 = membrane["r0"].as<double>();
      settings.membrane.f_zUp = membrane["f_zUp"].as<double>();
      SPDLOG_DEBUG("Generating membrane: position={} size={} distance={} mass={} velocity={} sigma={} epsilon={}",
                   ArrayUtils::to_string(x), ArrayUtils::to_string(n), distance, mass, ArrayUtils::to_string(v),
                   settings.membrane.sigma.value_or(1), epsilon.value_or(5));

      ParticleGenerator::membrane(particles, x, n, distance, mass, epsilon, settings.membrane.sigma, v,
                                  settings.membrane.upwardsParticles,
                                  upwardsParticlesIndexes.value_or(std::vector<std::array<int, 2>>{}));
    } else if (p["single"]) {
      YAML::Node single = p["single"];
      Vector3 x = single["position"].as<Vector3>();
      Vector3 v = single["velocity"].as<Vector3>();
      double mass = single["mass"].as<double>();
      std::optional<double> epsilon;
      std::optional<double> sigma;
      if (single["epsilon"]) epsilon = single["epsilon"].as<double>();
      if (single["sigma"]) sigma = single["sigma"].as<double>();

      if (single["force"]) {
        Vector3 f = single["force"].as<Vector3>();
        if (single["old_force"]) {
          Vector3 old_f = single["old_force"].as<Vector3>();
          SPDLOG_DEBUG(
              "Generating single particle: position={}, velocity={}, mass={}, sigma={}, epsilon={}, force={}, "
              "old_force={}",
              ArrayUtils::to_string(x), ArrayUtils::to_string(v), mass, sigma.value_or(1), epsilon.value_or(5),
              ArrayUtils::to_string(f), ArrayUtils::to_string(old_f));
          particles.emplace_back(x, v, mass, epsilon, sigma, f, old_f);
        } else {
          SPDLOG_DEBUG("Generating single particle: position={}, velocity={}, mass={}, sigma={}, epsilon={}, force={}",
                       ArrayUtils::to_string(x), ArrayUtils::to_string(v), mass, sigma.value_or(1), epsilon.value_or(5),
                       ArrayUtils::to_string(f));
          particles.emplace_back(x, v, mass, epsilon, sigma, f);
        }
      } else {
        SPDLOG_DEBUG("Generating single particle: position={} mass={} velocity={}, sigma={}, epsilon={}",
                     ArrayUtils::to_string(x), mass, ArrayUtils::to_string(v), sigma.value_or(1), epsilon.value_or(5));
        particles.emplace_back(x, v, mass, epsilon, sigma);
      }
    } else if (p["disc"]) {
      YAML::Node disc = p["disc"];
      Vector3 position = disc["position"].as<Vector3>();
      int radius = disc["radius"].as<int>();
      double distance = disc["distance"].as<double>();
      double mass = disc["mass"].as<double>();
      Vector3 velocity = disc["velocity"].as<Vector3>();
      std::optional<double> epsilon;
      std::optional<double> sigma;
      if (disc["epsilon"]) epsilon = disc["epsilon"].as<double>();
      if (disc["sigma"]) sigma = disc["sigma"].as<double>();

      SPDLOG_DEBUG("Generating disc: position={} radius={} distance={} mass={} velocity={}",
                   ArrayUtils::to_string(position), radius, distance, mass, ArrayUtils::to_string(velocity));
      ParticleGenerator::disc(particles, position, radius, distance, mass, epsilon, sigma, velocity);
    }
  }
}
