#ifndef REALPACK_INC_FR_HPP
#define REALPACK_INC_FR_HPP

// real headers
#include "z.hpp"

namespace real {

template <z_digit_container C>
struct fx {
  // base = 2 ^ bits_of<typename C::digit_type>()
  z<C> co;      // coefficient
  size_t nexp;  // exponent in negative
};

// requires: v != 0
// returns:
// 1. res <= floating point notation of (u/v),
// 2. | res - (u/v) | < base ^ (-n).
template <z_digit_container C>
fx<C> frac_n(const z<C>& u, const z<C>& v, size_t n) {
  fx<C> res;
  z<C> r;
  auto q = div_n(u, v, &r);
  shift_n(r, n);
  res.co = div_n<C>(r, v, nullptr);
  res.co.digits.insert(res.co.digits.end(), q.digits.begin(), q.digits.end());
  res.nexp = n;
  return res;
}

}  // namespace real

#endif  // !REALPACK_INC_FR_HPP
