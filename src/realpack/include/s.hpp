#ifndef REALPACK_INC_FR_HPP
#define REALPACK_INC_FR_HPP

// real headers
#include "z.hpp"

namespace real {

// fx: floating point represention for rational numbers with specified precisions.
template <z_digit_container C>
struct fx {
  // base = 2 ^ bits_of<typename C::digit_type>()
  z<C> coeff;   // coefficient
  size_t nexp;  // exponent in negative
};

// requires: v != 0
// returns:
// 1. res <= floating point notation of (u/v),
// 2. | res - (u/v) | < base ^ (-n).
template <z_digit_container C>
fx<C> frac(z<C> u, z<C> v, size_t n) {
  fx<C> res;
  z<C> r;
  bool sign = u.sign != v.sign;
  auto q = div_n(std::move(u), v, &r);
  shift_n(r, n);
  res.coeff = div_n<C>(std::move(r), std::move(v), nullptr);
  res.coeff.digits.insert(res.coeff.digits.end(), q.digits.begin(), q.digits.end());
  res.coeff.sign = sign;
  res.nexp = n;
  return res;
}

}  // namespace real

#endif  // !REALPACK_INC_FR_HPP
