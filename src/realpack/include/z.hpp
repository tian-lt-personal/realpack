#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

// std headers
#include <algorithm>
#include <concepts>
#include <limits>
#include <ranges>
#include <vector>

namespace real {

namespace details {

using z_default_container = std::vector<unsigned long>;
constexpr auto z_default_base = std::numeric_limits<typename z_default_container::value_type>::max();

}  // namespace details

template <class T>
concept z_digit_container = std::ranges::contiguous_range<T> && std::ranges::sized_range<T> && requires {
  typename T::value_type;
  sizeof(typename T::value_type) <= sizeof(size_t);
  std::is_unsigned_v<typename T::value_type>;
};

template <z_digit_container C = details::z_default_container, size_t B = details::z_default_base>
struct z {
  using container_type = C;
  using digit_type = typename C::value_type;

  C digits;
  bool sign = false;  // false for positive
};

template <z_digit_container C, size_t B>
constexpr bool is_zero(const z<C, B>& num) noexcept {
  if (num.digits.size() == 0) {
    return true;
  }
  return std::ranges::all_of(num.digits, [](typename z<C, B>::digit_type x) { return x == 0; });
}

template <z_digit_container C, size_t B, std::integral T>
constexpr z<C, B>& init_z(z<C, B>& num, T val) {
  if constexpr (std::is_unsigned_v<T>) {
    num.sign = false;
  } else {
    num.sign = val < 0;
    val = num.sign ? -val : val;
  }
  while (val > 0) {
    num.digits.push_back(val % B);
    val /= B;
  }
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_Z_HPP
