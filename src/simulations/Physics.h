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

/**
 * @brief Calculate the Lennard Jones force between two particles with some optimizations
 * @param p1 First particle
 * @param p2 Second particle
 * @param sigma2 precalculated sigma squared
 * @param epsilon_24 precalculated 24 * epsilon ɛ
 * @return Vector3
 */

inline Vector3 fastForce(Particle &p1, Particle &p2, double sigma2, double epsilon_24) {
  Vector3 diff = p1.getX() - p2.getX();

  // calculate norm^2 to prevent using square root
  double r2 = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];

  // calculate (sigma^2 / norm^2)
  // this is the base term for the next powers
  double inv_r2 = 1.0 / r2;
  double sig2_over_r2 = sigma2 * inv_r2;  // (sigma/r)^2

  // calculate (sigma/r)^6
  // value^6 = (value^2)^3
  double sig6_over_r6 = sig2_over_r2 * sig2_over_r2 * sig2_over_r2;

  // calculate force scalar
  // F = 24*eps/r^2 * ( 2*(sigma/r)^12 - (sigma/r)^6 )
  // Note: 24*eps is precomputed as epsilon24
  // Note: (sigma/r)^12 is just (sig6)^2

  double forceScalar = epsilon_24 * inv_r2 * (2.0 * sig6_over_r6 * sig6_over_r6 - sig6_over_r6);

  return forceScalar * diff;
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
namespace  harmonicPotential {
double sqrt2 = std::sqrt(2);

inline Vector3 forceStraight(Particle &p1, Particle &p2, double k, double r0) {
    Vector3 dist = p2.getX() - p1.getX(); //norm of x2 - x1 = norm of x1 - x2
    double norm = ArrayUtils::L2Norm(dist);
    Vector3 f = k * (norm - r0) * 1/norm * dist;
    return f;
}
inline Vector3 forceDiagonal(Particle &p1, Particle &p2, double k, double r0) {
  Vector3 dist = p2.getX() - p1.getX(); //norm of x2 - x1 = norm of x1 - x2
  double norm = ArrayUtils::L2Norm(dist);
  Vector3 f = k * (norm - sqrt2*r0) * 1/norm * dist;
  return f;
}
}

}  // namespace Physics
