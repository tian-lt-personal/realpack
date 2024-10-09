#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

// std headers
#include <algorithm>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <stack>
#include <string_view>
#include <vector>

namespace real {

namespace details {

using z_default_container = std::vector<unsigned long>;
constexpr auto z_default_base = std::numeric_limits<typename z_default_container::value_type>::max();

constexpr bool is_ws(char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f'; }
constexpr bool is_decimal(char ch) { return '0' <= ch && ch <= '9'; }
constexpr std::optional<unsigned> char2decimal(char ch) {
  if (is_decimal(ch))
    return static_cast<unsigned>(ch - '0');
  else
    return std::nullopt;
}

}  // namespace details

struct z_error : std::domain_error {
  z_error(const char* reason) : std::domain_error(reason) {}
};

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
  if (num.digits.size() == 0) return true;
  return std::ranges::all_of(num.digits, [](typename z<C, B>::digit_type x) { return x == 0; });
}

template <z_digit_container C, size_t B, std::integral T>
constexpr z<C, B>& init_z(z<C, B>& num, T val) {
  if constexpr (std::is_signed_v<T>) {
    num.sign = val < 0;
    val = num.sign ? -val : val;
  }
  while (val > 0) {
    num.digits.push_back(val % B);
    val /= B;
  }
  return num;
}

template <z_digit_container C, size_t B>
constexpr z<C, B>& init_z_decstr(z<C, B>& num, std::string_view str) {
  // illustration:
  // "    -       2024"
  //  sign | gap |num
  bool all0 = true;
  enum class parse_state { sign, gap, num, done };
  auto ps = parse_state::sign;
  std::vector<unsigned> digits;
  for (size_t i = 0; i < str.length() && ps != parse_state::done; ++i) {
    auto ch = str[i];
    switch (ps) {
      case parse_state::sign:
        if (details::is_ws(ch)) continue;
        switch (ch) {
          case '+':
            ps = parse_state::gap;
            break;
          case '-':
            num.sign = true;
            ps = parse_state::gap;
            break;
          default:
            if (details::is_decimal(ch)) {
              ps = parse_state::num;
              if (auto v = details::char2decimal(ch); v.has_value()) {
                all0 &= *v == 0;
                digits.push_back(*v);
              } else
                throw z_error{"bad number symbol."};
            } else
              throw z_error{"bad sign symbol."};
        }
        break;
      case parse_state::gap:
        if (details::is_ws(ch))
          continue;
        else {
          ps = parse_state::num;
        }
        [[fallthrough]];
      case parse_state::num: {
        if (details::is_ws(ch)) {
          ps = parse_state::done;
          break;
        }
        if (auto v = details::char2decimal(ch); v.has_value()) {
          all0 &= *v == 0;
          digits.push_back(*v);
        } else
          throw z_error{"bad number symbol."};
        break;
      }
      default:
        throw std::logic_error{"unhandled state."};
    }
  }
  if (!all0) {
    for (auto x : digits | std::views::reverse) {
    }
  }
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_Z_HPP
