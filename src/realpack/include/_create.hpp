#ifndef REALPACK_INC_CREATE_HPP
#define REALPACK_INC_CREATE_HPP

#include "z.hpp"

namespace real {

template <z_digit_container C = details::z_default_container>
z<C> create_z(std::integral auto i) {
  z<C> num;
  init(num, i);
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_CREATE_HPP
