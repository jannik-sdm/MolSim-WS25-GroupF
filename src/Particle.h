/**
 * @file Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <array>
#include <string>
/**
 * Objects of this class represent a Particle
 */

class Particle {
 private:
  /**
   * Position of the particle
   */
  std::array<double, 3> x;

  /**
   * Velocity of the particle
   */
  std::array<double, 3> v;

  /**
   * Force effective on this particle
   */
  std::array<double, 3> f;

  /**
   * Force which was effective on this particle
   */
  std::array<double, 3> old_f;

  /**
   * Mass of this particle
   */
  double m;

  /**
   * Type of the particle. Use it for whatever you want (e.g. to separate
   * molecules belonging to different bodies, matters, and so on)
   */
  int type;

 public:
  explicit Particle(int type = 0);

  Particle(const Particle &other);

  Particle(
      // for visualization, we need always 3 coordinates
      // -> in case of 2d, we use only the first and the second
      const std::array<double, 3> &x_arg, const std::array<double, 3> &v_arg, const double m_arg,
      const int type_arg = 0);

  virtual ~Particle();

  const std::array<double, 3> &getX() const;

  /**
   * @return Velocity of the particle
   */

  const std::array<double, 3> &getV() const;

  /**
   * @return Force effective on this particle
   */

  const std::array<double, 3> &getF() const;

  /**
   * @return Force which was effective on this particle \f$ \Delta t \f$ ago
   */

  const std::array<double, 3> &getOldF() const;

  /**
   * @return Mass of this particle
   */

  double getM() const;

  int getType() const;

  /**
   * @brief Sets new effective Force and updates Old Force to the current Force
   * @tparam new_f new Force - 3D-"Vector" (std::array<double, 3>)
   */

  void setF(const std::array<double, 3> &new_f);

  /**
   * @brief Adds the Parameter to the current Force and does NOT update old Force
   * @tparam partial_f \f$ \Delta F\f$ - 3D-"Vector" (std::array<double, 3>)
   */

  void addF(const std::array<double, 3> &partial_f);

  /**
   * @brief Adds the Parameter to the current Force and does NOT update old Force
   * @tparam partial_f \f$ \Delta F\f$ - 3D-"Vector" (std::array<double, 3>)
   */

  void subF(const std::array<double, 3> &partial_f);

  /**
   * @brief Sets the Position
   * @tparam new_x new Position - 3D-"Vector" (std::array<double, 3>)
   */

  void setX(const std::array<double, 3> &new_x);

  /**
   * @brief Sets the Velocity
   * @tparam new_v new Velocity - 3D-"Vector" (std::array<double, 3>)
   */
  void setV(const std::array<double, 3> &new_v);

  bool operator==(Particle &other);

  std::string toString() const;
};

std::ostream &operator<<(std::ostream &stream, Particle &p);
