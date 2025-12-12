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
  int num_particles = 0;

  std::string tmp_string;

  getline(file, tmp_string);
  spdlog::info("Read first line: {}", tmp_string);  // Könnte Sinvoll sein, deshalb auf info

  while (tmp_string.empty() or tmp_string[0] == '#') {
    getline(file, tmp_string);
    spdlog::debug("Read line: {}", tmp_string);  // Muss nicht den output zumüllen
  }

  std::istringstream numstream(tmp_string);
  numstream >> num_particles;
  spdlog::debug("Reading {}.", num_particles);
  getline(file, tmp_string);
  spdlog::info("Read line: {}", tmp_string);  // gibt guten Überblick

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
      spdlog::error("Error reading file: eof reached unexpectedly reading from line {}", i);
      exit(-1);
    }
    datastream >> m;
    particles.emplace_back(x, v, m);

    getline(file, tmp_string);
    spdlog::debug("Read line: {}", tmp_string);
  }
}
