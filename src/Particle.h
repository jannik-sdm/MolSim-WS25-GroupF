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
 * @brief Type alias of `std::array<double, 3>` for mathematical purposes
 *
 * This is a typealias to make it clear when we expect a Vector in a mathematical context
 */
using Vector3 = std::array<double, 3>;

/**
 * @class Particle
 * @brief Storage for position, velocity and force of a single Particle
 *
 * This class contains all physical properties of a particle.
 * For calculations in 2d, the last coordinate should just be left at zero
 */
class Particle {
 private:
  /**
   * Position of the particle
   */
  Vector3 x;

  /**
   * Velocity of the particle
   */
  Vector3 v;

  /**
   * Force effective on this particle
   */
  Vector3 f;

  /**
   * Force which was effective on this particle
   */
  Vector3 old_f;

  /**
   * Mass of this particle
   */
  double m;

  /**
   * Type of the particle. Use it for whatever you want (e.g. to separate
   * molecules belonging to different bodies, matters, and so on)
   */
  int type;

  /**
   * State of the particle.
   * -1 := dead
   * 0 := alive
   */
  int state;

 public:
  /**
   * @brief Empty constructor
   *
   * Constructs a new particle with all values set to zero
   *
   * @param state State of the particle
   * @see Particle::state
   */
  explicit Particle(int state = 0);

  /**
   * @brief Copy constructor
   *
   * Constructs a new particle by copying the values from another particle
   *
   * @param other Particle to copy from
   */
  Particle(const Particle &other);

  /**
   * @brief Move constructor
   *
   * Constructs a new particle by moving the old particle.
   * Don't use the other particle after calling this
   *
   * @param other Particle to move
   */
  Particle(Particle &&other) noexcept;

  Particle &operator=(const Particle &other) = default;
  Particle &operator=(Particle &&other) = default;

  /**
   * @brief Construct a new Particle object
   *
   * @param x_arg Position of the new particle
   * @param v_arg Velocity of the new particle
   * @param m_arg Mass of the new particle
   * @param type_arg Type of the new particle
   */
  Particle(const Vector3 &x_arg, const Vector3 &v_arg, const double m_arg, const Vector3 &f_arg = {0},
           const Vector3 &old_f_arg = {0}, const int type_arg = 0);

  /**
   * @brief Destroy the Particle object
   */
  virtual ~Particle();

  /**
   * @return Position of the particle
   */
  const Vector3 &getX() const;

  /**
   * @return Velocity of the particle
   */
  const Vector3 &getV() const;

  /**
   * @return Force effective on this particle
   */
  const Vector3 &getF() const;

  /**
   * @return Force which was effective on this particle \f$ \Delta t \f$ ago
   */
  const Vector3 &getOldF() const;

  /**
   * @return Mass of this particle
   */
  double getM() const;

  /**
   * @return Type of this particle
   */
  int getType() const;

  /**
   *
   * @return State of this particle
   */
  int getState() const;

  /**
   * @brief Sets new effective Force and updates Old Force to the current Force
   * @param new_f new Force - 3D-"Vector" (std::array<double, 3>)
   */
  void setF(const Vector3 &new_f);

  /**
   * @brief Adds the Parameter to the current Force and does NOT update old Force
   * @param partial_f \f$ \Delta F\f$ - 3D-"Vector" (std::array<double, 3>)
   */
  void addF(const Vector3 &partial_f);

  /**
   * @brief Adds the Parameter to the current Force and does NOT update old Force
   * @param partial_f \f$ \Delta F\f$ - 3D-"Vector" (std::array<double, 3>)
   */
  void subF(const Vector3 &partial_f);

  /**
   * @brief Sets the Position
   * @param new_x new Position - 3D-"Vector" (std::array<double, 3>)
   */
  void setX(const Vector3 &new_x);

  /**
   * @brief Sets the Velocity
   * @param new_v new Velocity - 3D-"Vector" (std::array<double, 3>)
   */
  void setV(const Vector3 &new_v);

  /**
   * @brief Sets the Mass
   * @param new_m
   */
  void setM(const double new_m);

  void setType(int new_type) { type = new_type; }

  void setState(int new_state) { state = new_state; }

  /**
   * @brief Comparte two particles
   *
   * This compares equality by comparing equality for all subcomponents (position, velocity, etc.)
   *
   * @param other
   * @return true
   * @return false
   */
  bool operator==(Particle &other);

  /**
   * @brief Convert this particle to a string
   *
   * Returns a string containing all properties of this particle
   *
   * @return std::string
   */
  std::string toString() const;
};

/**
 * @brief Write particle to stream
 *
 * Calls `toString()` on this particle and writes it to `stream`
 *
 * @param stream
 * @param p
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &stream, Particle &p);
