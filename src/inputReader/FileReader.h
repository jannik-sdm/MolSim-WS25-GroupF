#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "../Particle.h"
#include "spdlog/spdlog.h"

/**
 * @class FileReader
 * @brief Base Class for reading particle constellations from files
 *
 * Reading of files is done using the CRTP pattern. Every class that inherits
 * from this one needs to implement a static method `parse` that handles reading the file.
 * This should (?) allow for static polymorphism, evaluated at compile time
 *
 * @tparam T Derived class for CRTP pattern.
 * @note <T> must implement `static void parse(std::vector<Particle>&, std::ifstream&)`.
 *
 * @see https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
 */
template <typename T>
class FileReader {
 public:
  /**
   * @brief Fills a vector with particles based on instructions in an input file
   *
   * Checks the file for existence before delegating the actual parsing to the derived Type.
   * Exits with `-1` if the file can not be opened.
   *
   * @param particles Vector to fill with particles
   * @param filepath Path to input file to read.
   *
   */
  static void readFile(std::vector<Particle> &particles, const std::filesystem::path filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      spdlog::error("Error opening {}", filepath.string());
      exit(-1);
    }

    T::parse(particles, file);
  }
};
