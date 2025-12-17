#pragma once

#include "Particle.h"
#include "utils/ArrayUtils.h"

/**
 * @brief Namespace for physics formulas
 *
 */
namespace Physics {

namespace StoermerVerlet {
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
inline Vector3 position(Particle &p, double delta_t) {
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
inline Vector3 velocity(Particle &p, double delta_t) {
  const double coeff = 1 / (2 * p.getM());
  return p.getV() + delta_t * coeff * (p.getOldF() + p.getF());
}
}  // namespace StoermerVerlet

namespace Planet {
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
inline Vector3 force(Particle &p1, Particle &p2) {
  const double coeff = 1 / pow(ArrayUtils::L2Norm(p1.getX() - p2.getX()), 3);
  return coeff * p1.getM() * p2.getM() * (p2.getX() - p1.getX());
}
}  // namespace Planet

namespace LennardJones {

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
 * @param sigma σ
 * @param epsilon ɛ
 * @return Vector3
 */
inline Vector3 force(Particle &p1, Particle &p2, double sigma, double epsilon) {
  const double norm = ArrayUtils::L2Norm(p1.getX() - p2.getX());
  const double coeff_1 = std::pow(sigma / norm, 6) - 2 * std::pow(sigma / norm, 12);
  const double coeff_2 = -(24 * epsilon) / std::pow(norm, 2);
  return coeff_1 * coeff_2 * (p1.getX() - p2.getX());
}
}  // namespace LennardJones

namespace LorentzBerthelot {

/**
 * @brief Calculate σ for two particle types
 *
 * Calculate σ with formula
 * $\f[
 * \sigma_{ij} = \frac{\sigma_i + sigma_j}{2}
 * $\]
 *
 * @param sigma_i σ for first particle type
 * @param sigma_j σ for second particle type
 * @return σ after mixing rule
 */
inline double sigma(double sigma_i, double sigma_j) { return (sigma_i + sigma_j) / 2; }

/**
 * @brief Calculate ε for two particle types
 *
 * Calculates ε with the formula
 * \f[
 *  \epsilon_{ij} = \sqrt{\epsilon_i * \epsilon_j}
 * \f]
 *
 * @param epsilon_i ε for first particle type
 * @param epsilon_j ε for second particle type
 * @return ε after mixing rule
 */
inline double epsilon(double epsilon_i, double epsilon_j) { return std::sqrt(epsilon_i * epsilon_j); }
}  // namespace LorentzBerthelot

}  // namespace Physics
