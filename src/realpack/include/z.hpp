#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

// std headers
#include <algorithm>
#include <cassert>
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

constexpr bool is_ws(char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f'; }
constexpr bool is_decimal(char ch) { return '0' <= ch && ch <= '9'; }
constexpr std::optional<unsigned> char2decimal(char ch) {
  if (is_decimal(ch))
    return static_cast<unsigned>(ch - '0');
  else
    return std::nullopt;
}

template <std::unsigned_integral T>
struct promoted_digit;
template <>
struct promoted_digit<unsigned char> {
  using type = unsigned long;
};
template <>
struct promoted_digit<unsigned short> {
  using type = unsigned long;
};

}  // namespace details

struct z_error : std::domain_error {
  z_error(const char* reason) : std::domain_error(reason) {}
};

template <class T>
concept z_digit_container = std::ranges::contiguous_range<T> && std::ranges::sized_range<T> && requires {
  typename T::value_type;
  sizeof(typename T::value_type) <= sizeof(unsigned long);  // the largest digit type is `unsigned long`
  std::is_unsigned_v<typename T::value_type>;
};

template <z_digit_container C = details::z_default_container>
struct z {
  using container_type = C;
  using digit_type = typename C::value_type;

  C digits;           // least significant digits (LSD) first
  bool sign = false;  // false for positive
};

template <z_digit_container C>
constexpr bool is_zero(const z<C>& num) noexcept {
  if (num.digits.size() == 0) return true;
  return std::ranges::all_of(num.digits, [](typename z<C>::digit_type x) { return x == 0; });
}

// assumes: is_zero(num) == true && num.sign == false
template <z_digit_container C, std::integral T>
constexpr z<C>& init(z<C>& num, T val) {
  if (val == 0) return num;
  if constexpr (std::is_signed_v<T>) {
    num.sign = val < 0;
    val = num.sign ? -val : val;
  }
  constexpr auto digit_size = sizeof(typename z<C>::digit_type);
  if constexpr (sizeof(T) <= digit_size) {
    num.digits.push_back(val);
  } else {
    static_assert(digit_size < 4);
    constexpr unsigned long base = 1u << (digit_size * 8);
    while (val > 0) {
      num.digits.push_back(val % base);
      val /= base;
    }
  }
  return num;
}

// assumes: both lhs and rhs are non-negative integers
// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <z_digit_container C>
constexpr int cmp_n(const z<C>& lhs, const z<C>& rhs) {
  assert(lhs.sign == false && rhs.sign == false);
  if (lhs.digits.size() != rhs.digits.size()) return lhs.digits.size() < rhs.digits.size() ? -1 : 1;
  auto l = lhs.digits.crbegin();
  auto r = rhs.digits.crbegin();
  for (; l != lhs.digits.crend(); ++l, ++r) {
    if (*l != *r) {
      return *l < *r ? -1 : 1;
    }
  }
  return 0;
}

// assumes: both lhs and rhs are non-negative integers
// returns: r = lhs + rhs;
template <z_digit_container C>
constexpr z<C> add_n(const z<C>& lhs, const z<C>& rhs) {
  using D = typename z<C>::digit_type;
  z<C> r;
  // ensure a <= b
  auto& a = lhs.digits.size() < rhs.digits.size() ? lhs : rhs;
  auto& b = lhs.digits.size() < rhs.digits.size() ? rhs : lhs;
  auto i = 0;
  D cy = 0;
  for (; i < a.digits.size(); ++i) {
    auto t = a.digits[i] + b.digits[i] + cy;
    cy = t >= a.digits[i] && t >= b.digits[i] ? 0 : 1;
    r.digits.push_back(t);
  }
  for (; i < b.digits.size(); ++i) {
    auto t = b.digits[i] + cy;
    cy = t >= b.digits[i] && t >= cy ? 0 : 1;
    r.digits.push_back(t);
  }
  if (cy > 0) {
    r.digits.push_back(1u);
  }
  return r;
}

template <z_digit_container C>
constexpr z<C>& init_decstr(z<C>& num, std::string_view str) {
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
    // TODO: convert the base 10 number to other bases.
  }
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_Z_HPP
