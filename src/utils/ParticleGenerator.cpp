#include "ParticleGenerator.h"

#include <spdlog/spdlog.h>

#include "Particle.h"
#include "utils/ArrayUtils.h"

void ParticleGenerator::cuboid(std::vector<Particle> &particles, const Vector3 pos, const std::array<unsigned int, 3> n,
                               const double distance, const double mass, const std::optional<double> epsilon,
                               std::optional<double> sigma, const Vector3 v) {
  // Reserve space for new particles
  spdlog::trace("Reserving {} particles", n[0] * n[1] * n[2]);
  particles.reserve(particles.size() + n[0] * n[1] * n[2]);

  // Generate cuboid
  for (unsigned int x = 0; x < n[0]; x += 1)
    for (unsigned int y = 0; y < n[1]; y += 1)
      for (unsigned int z = 0; z < n[2]; z += 1) {
        Vector3 new_pos = pos + distance * std::array<double, 3>{static_cast<double>(x), static_cast<double>(y),
                                                                 static_cast<double>(z)};
        particles.emplace_back(new_pos, v, mass, epsilon, sigma);
      }
}


void ParticleGenerator::membrane(std::vector<Particle> &particles, const Vector3 pos, const std::array<unsigned int, 3> n,
                                 const double distance, const double mass, const std::optional<double> epsilon,
                                 std::optional<double> sigma, Vector3 v, std::vector<Particle *> upwardsParticles,
                                 std::vector<std::array<int, 2>> upwardsParticlesIndexes) {
  // Reserve space for new particles
  spdlog::trace("Reserving {} particles", n[0] * n[1] * n[2]);
  particles.reserve(particles.size() + n[0] * n[1] * n[2]);

  // Generate cuboid
  Particle *rowsFirst = nullptr;
  Particle *left = nullptr;
  Particle *lower = nullptr;
  for (unsigned int y = 0; y < n[1]; y += 1)
    for (unsigned int x = 0; x < n[0]; x += 1){
      //Membranes can only be flat, so the z-offset to the start Position is always 0
        Vector3 new_pos = pos + distance * std::array<double, 3>{static_cast<double>(x), static_cast<double>(y),
                                                                 0.0};
        particles.emplace_back(new_pos, v, mass, epsilon, sigma);
      //Set Neighbors
      //           y   +-------------+---------+--------------+
      //           ^   | Left Upper  | Upper   | Right Upper  |
      //           |   |     0       |    1    |      2       |
      //           |   +-------------+---------+--------------+
      //           |   | Left        |  OWN    | Right        |
      //           |   |     3       |         |      4       |
      //           |   +-------------+---------+--------------+
      //           |   | Left Lower  | Lower   | Right Lower  |
      //           |   |     5       |    6    |      7       |
      //           |   +-------------+---------+--------------+
      //           ----------------------------------------> x

        Particle *current = &particles.back();
      //Set the lower one to the last lower one and update the first in a row to the current Particle
      if (x == 0) {
        lower = rowsFirst;
        rowsFirst = current;
      }
      //Set lower for current
      current->setNeighbour(lower, 6);
      //Set Left for current
      current->setNeighbour(left, 3);
      //Set right for left
      if (left != nullptr) left->setNeighbour(current,4);

      if (lower != nullptr) {
        //Set upper for lower
        lower->setNeighbour(current, 1);
        //Set right lower for current
        Particle *rightLower = lower->getNeighbor(4);
        current->setNeighbour(rightLower,7);
        //Set left upper for right lower
        if (rightLower != nullptr) rightLower->setNeighbour(current,0);
        //Set left lower for current
        Particle *leftLower = lower->getNeighbor(3);
        current->setNeighbour(leftLower,5);
        //Set right upper for left lower
        if (leftLower != nullptr) leftLower->setNeighbour(current,2);
      }
      // Update left
      if (x != n[1] -1) left = current;
      else left = nullptr;


      //Save upwardsParticles
      if (std::find(upwardsParticlesIndexes.begin(), upwardsParticlesIndexes.end(), std::array<int, 2>{static_cast<int>(x), static_cast<int>(y)}) != upwardsParticlesIndexes.end()) {
        upwardsParticles.emplace_back(current);
      }
      }

}

void ParticleGenerator::disc(std::vector<Particle> &particles, const Vector3 position, const int radius,
                             const double distance, const double mass, const std::optional<double> epsilon,
                             std::optional<double> sigma, const Vector3 velocity) {
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
      particles.emplace_back(new_pos, velocity, mass, epsilon, sigma);
    }
  }

  // In case that we reserved too much space. Should not really happen
  particles.shrink_to_fit();
}
