#ifndef REALPACK_INC_FR_HPP
#define REALPACK_INC_FR_HPP

// real headers
#include "z.hpp"

namespace real {

template <z_digit_container C>
struct fx {
  // base = 2 ^ bits_of<C::digit_type>()
  z<C> co;     // coefficient
  size_t exp;  // exponent
};

// requires: v != 0
// returns:
// 1. res <= floating point notation of (u/v),
// 2. | res - (u/v) | < base ^ (-n).
template <z_digit_container C>
fx<C> frac_n(const z<C>& u, const z<C>& v, size_t n) {
  (void)n;
  z<C> r;
  auto q = div_n(u, v, &r);

  return {};
}

}  // namespace real

#endif  // !REALPACK_INC_FR_HPP
