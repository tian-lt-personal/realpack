#ifndef REALPACK_INC_OP_HPP
#define REALPACK_INC_OP_HPP

#include "z.hpp"

template <real::z_digit_container C>
constexpr real::z<C> operator+(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::add(lhs, rhs);
}

template <real::z_digit_container C>
constexpr real::z<C> operator-(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::add(lhs, real::neg(rhs));
}

template <real::z_digit_container C>
constexpr real::z<C> operator*(const real::z<C>& lhs, const real::z<C>& rhs) {
  return real::mul(lhs, rhs);
}

template <real::z_digit_container C>
constexpr real::z<C> operator/(real::z<C> lhs, real::z<C> rhs) {
  return real::div(std::move(lhs), std::move(rhs));
}

template <real::z_digit_container C>
constexpr real::z<C> operator%(real::z<C> lhs, real::z<C> rhs) {
  real::z<C> r;
  real::div(std::move(lhs), std::move(rhs), &r);
  return r;
}

#endif  // !REALPACK_INC_OP_HPP
