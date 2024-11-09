#ifndef REALPACK_INC_OP_HPP
#define REALPACK_INC_OP_HPP

#include "z.hpp"

template <real::z_digit_container C>
constexpr real::z<C> operator+(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::add_z(lhs, rhs);
}

template <real::z_digit_container C>
constexpr real::z<C> operator-(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::add_z(lhs, real::neg_z(rhs));
}

template <real::z_digit_container C>
constexpr real::z<C> operator*(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::mul_z(lhs, rhs);
}

template <real::z_digit_container C>
constexpr real::z<C> operator/(real::z<C> lhs, real::z<C> rhs) {
  return real::div_z(std::move(lhs), std::move(rhs));
}

template <real::z_digit_container C>
constexpr real::z<C> operator%(real::z<C> lhs, real::z<C> rhs) {
  real::z<C> r;
  real::div_z(std::move(lhs), std::move(rhs), &r);
  return r;
}

#endif  // !REALPACK_INC_OP_HPP
