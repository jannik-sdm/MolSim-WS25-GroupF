#include "XYZReader.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

void XYZReader::parse(std::vector<Particle> &particles, std::ifstream &file) {
  std::array<double, 3> x;
  std::array<double, 3> v;
  double m;
  int num_particles = 0;

  std::string tmp_string;

  if (file.is_open()) {
    getline(file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

    while (tmp_string.empty() or tmp_string[0] == '#') {
      getline(file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }

    std::istringstream numstream(tmp_string);
    numstream >> num_particles;
    std::cout << "Reading " << num_particles << "." << std::endl;
    getline(file, tmp_string);
    std::cout << "Read line: " << tmp_string << std::endl;

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
        std::cout << "Error reading file: eof reached unexpectedly reading from line " << i << std::endl;
        exit(-1);
      }
      datastream >> m;
      particles.emplace_back(x, v, m);

      getline(file, tmp_string);
      std::cout << "Read line: " << tmp_string << std::endl;
    }
  } else {
    std::cout << "Error: could not open file " << std::endl;
    exit(-1);
  }
}
