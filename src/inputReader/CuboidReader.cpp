#include "CuboidReader.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "../ParticleGenerator.h"

void CuboidReader::parse(std::vector<Particle> &particles, std::istream &file) {
  Vector3 x;
  std::array<unsigned int, 3> n;
  Vector3 v;
  double distance;
  double mass;

  std::string current_line;

  while (std::getline(file, current_line)) {
    if (current_line.empty() || current_line[0] == '#') continue;

    std::istringstream data(current_line);

    for (auto &xₖ : x) data >> xₖ;
    for (auto &nₖ : n) data >> nₖ;

    data >> distance;
    data >> mass;

    for (auto &vₖ : v) data >> vₖ;

    ParticleGenerator::cuboid(particles, x, n, distance, mass, v);
  }
}
