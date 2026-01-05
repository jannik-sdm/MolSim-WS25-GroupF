#include "ParticleGenerator.h"

#include <spdlog/spdlog.h>

#include "Particle.h"
#include "utils/ArrayUtils.h"

void ParticleGenerator::cuboid(std::vector<Particle> &particles, const Vector3 pos, const std::array<unsigned int, 3> n,
                               const double distance, const double mass, const Vector3 v) {
  // Reserve space for new particles
  spdlog::trace("Reserving {} particles", n[0] * n[1] * n[2]);
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

void ParticleGenerator::disc(std::vector<Particle> &particles, const Vector3 position, const int radius,
                             const double distance, const double mass, const Vector3 velocity) {
  // A Disk covers at most ¼π ≈ 78.54% of the original grid
  const unsigned int particles_to_reserve = (radius * 2 * radius * 2 * 78) / 100;
  spdlog::trace("Reserving {} particles", particles_to_reserve);
  particles.reserve(particles.size() + particles_to_reserve);

  // Generate disk
  for (int x = -radius + 1; x < radius; x += 1) {
    for (int y = -radius + 1; y < radius; y += 1) {
      Vector3 displacement = distance * std::array<double, 3>{static_cast<double>(x), static_cast<double>(y), 0};

      // We compare with `radius - 1` because the first particle is at the relative position (0,0)
      if (ArrayUtils::L2Norm(displacement) > static_cast<double>(radius - 1) * distance) continue;

      Vector3 new_pos = position + displacement;
      particles.emplace_back(new_pos, velocity, mass);
    }
  }

  // In case that we reserved too much space. Should not really happen
  particles.shrink_to_fit();
}
