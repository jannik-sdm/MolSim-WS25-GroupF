#pragma once

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>

#include "Particle.h"
#include "Settings.h"

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

namespace YAML {

template <>
struct convert<Particle> {
  static Node encode(const Particle &rhs) {
    Node node;

    node["position"] = rhs.getX();
    node["velocity"] = rhs.getV();
    node["force"] = rhs.getF();
    node["old_force"] = rhs.getOldF();
    node["mass"] = rhs.getM();
    node["type"] = rhs.getType();

    return node;
  }

  static bool decode(const Node &node, Particle &rhs) {
    if (!node.IsMap()) {
      return false;
    }

    auto position = node["position"];
    if (!position) return false;

    auto velocity = node["velocity"];
    if (!velocity) return false;

    auto mass = node["mass"];
    if (!mass) return false;

    auto force = node["force"];
    auto old_force = node["old_force"];
    auto type = node["type"];

    if (force && old_force && type)
      rhs = Particle(position.as<Vector3>(), velocity.as<Vector3>(), mass.as<double>(), force.as<Vector3>(),
                     old_force.as<Vector3>(), type.as<double>());
    else if (force && old_force) {
      rhs = Particle(position.as<Vector3>(), velocity.as<Vector3>(), mass.as<double>(), force.as<Vector3>(),
                     old_force.as<Vector3>());
    } else if (force) {
      rhs = Particle(position.as<Vector3>(), velocity.as<Vector3>(), mass.as<double>(), force.as<Vector3>());
    } else {
      rhs = Particle(position.as<Vector3>(), velocity.as<Vector3>(), mass.as<double>());
    }

    return true;
  }
};

template <>
struct convert<Settings::Output> {
  static Node encode(const Settings::Output &rhs) {
    Node node;

    node["directory"] = rhs.directory.string();
    node["prefix"] = rhs.prefix;

    if (rhs.export_filename) node["export_filename"] = rhs.export_filename.value().string();

    node["frequency"] = rhs.frequency;

    auto log_level = spdlog::level::to_string_view(rhs.log_level);
    node["log_level"] = std::string(log_level.data(), log_level.size());

    return node;
  }

  static bool decode(const Node &node, Settings::Output &rhs) {
    if (!node.IsMap()) {
      return false;
    }

    auto directory = node["directory"];
    if (directory) {
      rhs.directory = std::filesystem::path(directory.as<std::string>());
    } else {
      // fallback to old key
      auto folder = node["folder"];
      if (folder) rhs.directory = std::filesystem::path(folder.as<std::string>());
    }

    auto prefix = node["prefix"];
    if (prefix) rhs.prefix = std::filesystem::path(prefix.as<std::string>());

    auto export_filename = node["export_filename"];
    if (export_filename) rhs.export_filename = std::filesystem::path(export_filename.as<std::string>());

    auto frequency = node["frequency"];
    if (frequency) rhs.frequency = frequency.as<unsigned int>();

    auto log_level = node["log_level"];
    if (log_level) rhs.log_level = spdlog::level::from_str(log_level.as<std::string>());

    return true;
  }
};

template <>
struct convert<Settings::Simulation> {
  static Node encode(const Settings::Simulation &rhs) {
    Node node;

    if (rhs.worksheet) node["worksheet"] = rhs.worksheet.value();

    if (rhs.end_time) node["end_time"] = rhs.end_time.value();
    if (rhs.delta_t) node["delta_t"] = rhs.delta_t.value();

    if (rhs.domain) node["domain"] = rhs.domain.value();
    if (rhs.cutoff_radius) node["cutoff_radius"] = rhs.cutoff_radius.value();
    if (rhs.borders) {
      auto n = node["borders"];

      for (auto border : rhs.borders.value()) {
        switch (border) {
          case BorderType::OUTFLOW:
            n.push_back("outflow");
            break;

          case BorderType::REFLECTION:
            n.push_back("reflection");
            break;

          case BorderType::PERIODIC:
            n.push_back("periodic");
            break;

          default:
            break;
        }
      }
    }

    if (rhs.is2D) node["dimension"] = "2D";
    if (rhs.brown_motion_avg_velocity) node["brown_motion_avg_velocity"] = rhs.brown_motion_avg_velocity.value();
    if (rhs.t_initial) node["temp_initial"] = rhs.t_initial.value();
    if (rhs.t_final) node["temp_final"] = rhs.t_final.value();
    if (rhs.t_max_change) node["temp_max_change"] = rhs.t_max_change.value();
    if (rhs.t_frequency) node["temp_frequency"] = rhs.t_frequency.value();
    return node;
  }

  static bool decode(const Node &node, Settings::Simulation &rhs) {
    if (!node.IsMap()) {
      return false;
    }

    auto worksheet = node["worksheet"];
    if (worksheet) rhs.worksheet = worksheet.as<unsigned int>();

    auto end_time = node["end_time"];
    if (end_time) rhs.end_time = end_time.as<double>();

    auto delta_t = node["delta_t"];
    if (delta_t) rhs.delta_t = delta_t.as<double>();

    auto brown_motion_avg_velocity = node["brown_motion_avg_velocity"];
    if (brown_motion_avg_velocity) rhs.brown_motion_avg_velocity = brown_motion_avg_velocity.as<double>();

    auto cutoff_radius = node["cutoff_radius"];
    if (cutoff_radius) rhs.cutoff_radius = cutoff_radius.as<double>();

    auto domain = node["domain"];
    if (domain) rhs.domain = domain.as<Vector3>();

    auto borders = node["borders"];
    if (borders) {
      auto border_strings = borders.as<std::array<std::string, 6>>();
      std::array<BorderType, 6> border_enums;
      std::transform(border_strings.begin(), border_strings.end(), border_enums.begin(), string_to_border_type);
      rhs.borders = border_enums;
    }

    auto dimension = node["dimension"];
    if (dimension) rhs.is2D = dimension.as<std::string>() == "2D";

    auto t_init = node["temp_initial"];
    if (t_init) rhs.t_initial = t_init.as<double>();

    auto t_final = node["temp_final"];
    if (t_final) rhs.t_final = t_final.as<double>();
    else if (t_init) rhs.t_final = node["t_initial"].as<double>();

    auto t_max_step = node["temp_max_change"];
    if (t_max_step) rhs.t_max_change = t_max_step.as<double>();

    auto frequency = node["temp_frequency"];
    if (frequency) rhs.t_frequency = frequency.as<unsigned int>();

    return true;
  }
};
}  // namespace YAML
