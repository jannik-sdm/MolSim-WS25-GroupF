#pragma once

#include "Particle.h"
#include "utils/ArrayUtils.h"

/**
 * @brief Namespace for physics formulas
 *
 */
namespace Physics {

/**
 * @brief Calculate the force between two planets
 *
 * Calculate the force between two planets with the formula
 * \f[
 *   F_{ij} = \frac{m_im_j}{(||x_i-x_j||_2)^3}(x_j-x_i)
 * \f].
 *
 * @param p1
 * @param p2
 * @return Vector3
 */
inline Vector3 planetForce(Particle &p1, Particle &p2) {
  const double coeff = 1 / pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 3);
  return coeff * p1.getM() * p2.getM() * (p2.getX() - p1.getX());
}

/**
 * @brief Calculate the Lennard Jones force between two particles
 *
 * Calculate the lennard jones force between two particles with the formula
 * \f[
 *   F_{ij} = * - \frac{24 \cdot \epsilon}{(\|x_i - x_j\|_2)^2}
 *   (( \frac{\sigma}{\|x_i - x_j\|_2})^6
 *   - 2 (\frac{\sigma}{\|x_i - x_j\|_2})^{12}) (x_i - x_j)
 * \f]
 *
 * @param p1 First particle
 * @param p2 Second particle
 * @param delta_t Δt
 * @param sigma σ
 * @param epsilon ɛ
 * @return Vector3
 */
inline Vector3 lennardJonesForce(Particle &p1, Particle &p2, double sigma, double epsilon) {
  const double norm = ArrayUtils::L2Norm(p1.getX() - p2.getX());
  const double coeff_1 = std::pow(sigma / norm, 6) - 2 * std::pow(sigma / norm, 12);
  const double coeff_2 = -(24 * epsilon) / std::pow(norm, 2);
  return coeff_1 * coeff_2 * (p1.getX() - p2.getX());
}

/**
 * @brief Calculate the new position
 *
 * Calculate the new position of a particle with the formula
 * \f[
 *   x_i(t_{n+1}) = x_i(t_n)+\Delta t \cdot v_i(t_n) + (\Delta t)^2 \frac{F_i(t_n)}{2m_i}
 * \f]
 *
 * @param p
 * @param delta_t
 * @return Vector3
 */
inline Vector3 calculateX(Particle &p, double delta_t) {
  const double coeff = 1 / (2 * p.getM());
  return p.getX() + delta_t * p.getV() + pow(delta_t, 2) * coeff * p.getF();
}

/**
 * @brief Calculate new Velocity for a particle
 *
 * Calculate the new Velocity of a particle with the formula
 * \f[
 *   v_i(t_{n+1}) = v_i(t_n) + \Delta t \frac{F_i(t_n)+F_i(t_{n+1})}{2m_i}
 * \f]
 *
 * @param p
 * @param delta_t
 * @return Vector3
 */
inline Vector3 calculateV(Particle &p, double delta_t) {
  const double coeff = 1 / (2 * p.getM());
  return p.getV() + delta_t * coeff * (p.getOldF() + p.getF());
}

}  // namespace Physics
