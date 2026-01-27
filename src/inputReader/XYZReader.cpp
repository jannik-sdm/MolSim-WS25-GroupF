#include "inputReader/XYZReader.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

void XYZReader::parse(std::vector<Particle> &particles, std::istream &file) {
  std::array<double, 3> x;
  std::array<double, 3> v;
  double m;
  std::optional<double> epsilon;
  std::optional<double> sigma;
  int num_particles = 0;

  std::string tmp_string;

  getline(file, tmp_string);
  SPDLOG_INFO("Read first line: {}", tmp_string);  // Könnte Sinvoll sein, deshalb auf info

  while (tmp_string.empty() or tmp_string[0] == '#') {
    getline(file, tmp_string);
    SPDLOG_DEBUG("Read line: {}", tmp_string);  // Muss nicht den output zumüllen
  }

  std::istringstream numstream(tmp_string);
  numstream >> num_particles;
  SPDLOG_DEBUG("Reading {}.", num_particles);
  getline(file, tmp_string);
  SPDLOG_INFO("Read line: {}", tmp_string);  // gibt guten Überblick

  particles.reserve(num_particles);

  for (int i = 0; i < num_particles; i++) {
    std::istringstream datastream(tmp_string);

    for (auto &xj : x) {
      datastream >> xj;
    }
    for (auto &vj : v) {
      datastream >> vj;
    }
    if (datastream.eof()) {
      SPDLOG_ERROR("Error reading file: eof reached unexpectedly reading from line {}", i);
      exit(-1);
    }
    datastream >> m;
    particles.emplace_back(x, v, m, epsilon, sigma);

    getline(file, tmp_string);
    SPDLOG_DEBUG("Read line: {}", tmp_string);
  }
}
