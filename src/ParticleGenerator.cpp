#include "ParticleGenerator.h"

// #include <iostream>

#include "Particle.h"
#include "utils/ArrayUtils.h"

void ParticleGenerator::cuboid(std::vector<Particle> &particles, Vector3 pos, std::array<unsigned int, 3> n,
                               double distance, double mass, Vector3 v) {
  // Reserve space for new particles
  particles.reserve(particles.size() + n[0] * n[1] * n[2]);

  // Generate cuboid
  for (unsigned int x = 0; x < n[0]; x += 1)
    for (unsigned int y = 0; y < n[1]; y += 1)
      for (unsigned int z = 0; z < n[2]; z += 1) {
        Vector3 new_pos = pos + distance * std::array<double, 3>{static_cast<double>(x), static_cast<double>(y),
                                                                 static_cast<double>(z)};
        particles.emplace_back(new_pos, v, mass);
      }
}
